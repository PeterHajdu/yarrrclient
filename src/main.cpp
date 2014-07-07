#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <map>

#include <yarrr/object.hpp>
#include <yarrr/clock_synchronizer.hpp>
#include <yarrr/event.hpp>
#include <yarrr/login.hpp>
#include <yarrr/command.hpp>
#include <yarrr/ship_control.hpp>
#include <yarrr/object_state_update.hpp>
#include <yarrr/event_factory.hpp>

#include <thenet/service.hpp>
#include <thenet/address.hpp>

#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>

#include <thectci/dispatcher.hpp>

#include "sdl_engine.hpp"
#include <SDL2/SDL.h>

namespace
{
  class Client
  {
    public:
      Client(
          the::net::Connection& connection,
          the::ctci::Dispatcher& dispatcher )
        : m_connection( connection )
        , m_dispatcher( dispatcher )
        , m_logged_in( false )
        , ship_id( 0 )
      {
        m_dispatcher.register_listener< yarrr::LoginResponse >(
            std::bind( &Client::handle_login_response, this, std::placeholders::_1 ) );
        m_dispatcher.register_listener<yarrr::ObjectStateUpdate>(
            std::bind( &Client::handle_object_state_update, this, std::placeholders::_1 ) );
      }

      void handle_incoming_messages()
      {
        the::net::Data message;
        while ( m_connection.receive( message ) )
        {
          yarrr::Event::Pointer event( yarrr::EventFactory::create( message ) );
          if ( !event )
          {
            continue;
          }
          m_dispatcher.polymorphic_dispatch( *event );
        }
      }

      void send( yarrr::Data&& data )
      {
        m_connection.send( std::move( data ) );
      }

      void log_in()
      {
        m_connection.send( yarrr::LoginRequest( "appletree" ).serialize() );
        while ( !m_logged_in )
        {
          handle_incoming_messages();
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }
      }

      void handle_login_response( const yarrr::LoginResponse& response )
      {
        ship_id = response.object_id();
      }

      void handle_object_state_update( const yarrr::ObjectStateUpdate& object_state_update )
      {
        const yarrr::Object& ship( object_state_update.object() );
        if ( ship.id == ship_id )
        {
          m_logged_in = true;
        }
      }

    private:
      the::net::Connection& m_connection;
      the::ctci::Dispatcher& m_dispatcher;
      bool m_logged_in;

    public:
      yarrr::Object::Id ship_id;
  };


  class ConnectionEstablisher
  {
    public:
      ConnectionEstablisher(
          the::time::Clock& clock,
          const the::net::Address& address,
          the::ctci::Dispatcher& dispatcher )

        : m_network_service(
          std::bind( &ConnectionEstablisher::new_connection, this, std::placeholders::_1 ),
          std::bind( &ConnectionEstablisher::lost_connection, this, std::placeholders::_1 ) )
        , m_clock( clock )
        , m_clock_synchronizer( nullptr )
        , m_dispatcher( dispatcher )
      {
        std::cout << "connecting to host: " << address.host << ", port: " << address.port << std::endl;
        m_network_service.connect_to( address );
        m_network_service.start();
      }


      Client& wait_for_connection()
      {
        while ( !m_client )
        {
          std::cout << "connecting..." << std::endl;
          std::lock_guard< std::mutex > connection_guard( m_client_mutex );
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }

        while ( !m_clock_synchronizer->clock_offset() )
        {
          std::cout << "waiting for clock synchronization..." << std::endl;
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }
        std::cout << "network latency: " << m_clock_synchronizer->network_latency() << std::endl;
        std::cout << "clock offset: " << m_clock_synchronizer->clock_offset() << std::endl;
        m_clock_synchronizer->synchronize_local_clock();

        return *m_client;
      }

      void new_connection( the::net::Connection& connection )
      {
        std::unique_ptr< ClockSync > clock_synchronizer(
            new yarrr::clock_sync::Client< the::time::Clock, the::net::Connection >(
              m_clock,
              connection ) );
        m_clock_synchronizer = clock_synchronizer.get();
        connection.register_task( std::move( clock_synchronizer ) );

        std::lock_guard< std::mutex > connection_guard( m_client_mutex );
        std::cout << "new connection established" << std::endl;
        m_client.reset( new Client( connection, m_dispatcher ) );
      }

      void lost_connection( the::net::Connection& )
      {
        std::cout << "connection lost" << std::endl;
        exit( 1 );
      }

    private:
      the::net::Service m_network_service;
      std::unique_ptr< Client > m_client;
      std::mutex m_client_mutex;
      the::time::Clock& m_clock;
      typedef yarrr::clock_sync::Client< the::time::Clock, the::net::Connection > ClockSync;
      typedef ClockSync* ClockSyncPointer;
      ClockSyncPointer m_clock_synchronizer;
      the::ctci::Dispatcher& m_dispatcher;
  };

}

class DrawableShip : public DrawableObject
{
  public:
    DrawableShip( SdlEngine& graphics_engine )
      : DrawableObject( graphics_engine )
      , m_local_ship_control( m_local_ship )
    {
    }

    void in_focus()
    {
      m_graphical_engine.focus_to( m_local_ship.coordinate );
    }

    void handle_command( const yarrr::Command& command )
    {
      m_local_ship_control.handle_command( command );
    }

    void update_ship( const yarrr::Object& ship )
    {
      m_network_ship = ship;
    }

    void travel_in_time_to( const the::time::Clock::Time& timestamp )
    {
      yarrr::travel_in_time_to( timestamp, m_local_ship );
      yarrr::travel_in_time_to( timestamp, m_network_ship );
      m_local_ship.coordinate = ( m_network_ship.coordinate + m_local_ship.coordinate ) * 0.5;
      m_local_ship.velocity = ( m_network_ship.velocity + m_local_ship.velocity ) * 0.5;
      m_local_ship.angle = ( m_network_ship.angle + m_local_ship.angle ) * 0.5;
      m_local_ship.vangle = ( m_network_ship.vangle + m_local_ship.vangle ) * 0.5;
      std::cout << "network state: " << m_network_ship;
      std::cout << "local state: " << m_local_ship;
    }

    void draw() override
    {
      std::cout << m_local_ship << std::endl;
      m_graphical_engine.draw_ship( m_local_ship );
    }

  private:

    yarrr::Object m_local_ship;
    yarrr::Object m_network_ship;
    yarrr::ShipControl m_local_ship_control;
};

int main( int argc, char ** argv )
{
  typedef std::map< int, std::unique_ptr< DrawableShip > > ShipContainer;
  ShipContainer ships;

  SdlEngine graphics_engine( 1024, 768 );

  the::ctci::Dispatcher event_dispatcher;

  event_dispatcher.register_listener<yarrr::ObjectStateUpdate>(
      [ &graphics_engine, &ships ]( const yarrr::ObjectStateUpdate& object_state_update )
      {
        const yarrr::Object& ship( object_state_update.object() );
        ShipContainer::iterator drawable_ship( ships.find( ship.id ) );
        if ( drawable_ship == ships.end() )
        {
          ships.emplace( std::make_pair(
                ship.id,
                std::unique_ptr< DrawableShip >( new DrawableShip( graphics_engine ) ) ) );
        }

        ships[ ship.id ]->update_ship( ship );
      } );

  the::time::Clock clock;
  ConnectionEstablisher establisher(
      clock,
      the::net::Address(
        argc > 1 ?
        argv[1] :
        "localhost:2001"),
      event_dispatcher );
  Client& client( establisher.wait_for_connection() );
  client.log_in();
  DrawableShip& my_ship( *ships.find( client.ship_id )->second );

  the::time::FrequencyStabilizer< 60, the::time::Clock > frequency_stabilizer( clock );

  bool running( true );
  while ( running )
  {
    the::time::Clock::Time now( clock.now() );
    std::cout << "time: " << now << std::endl;
    SDL_Event event;
    while ( SDL_PollEvent( &event ) )
    {
      if ( event.type == SDL_QUIT )
      {
        running = false;
      }
      else if ( event.type == SDL_KEYDOWN )
      {
        char cmd( 0 );
        switch( event.key.keysym.sym )
        {
          case SDLK_q:
            running = false;
            break;
          case SDLK_UP:
            cmd = yarrr::Command::thruster;
            break;
          case SDLK_LEFT:
            cmd = yarrr::Command::ccw;
            break;
          case SDLK_RIGHT:
            cmd = yarrr::Command::cw;
            break;
        }

        yarrr::Command command( cmd, now );
        my_ship.handle_command( command );
        client.send( command.serialize() );
      }
    }

    client.handle_incoming_messages();

    for ( auto& ship : ships )
    {
      ship.second->travel_in_time_to( now );
    }

    my_ship.in_focus();
    graphics_engine.update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

