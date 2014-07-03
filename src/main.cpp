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
#include <yarrr/object_state_update.hpp>

#include <thenet/service.hpp>
#include <thenet/address.hpp>

#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>

#include <thectci/factory.hpp>
#include <thectci/dispatcher.hpp>

#include "sdl_engine.hpp"
#include <SDL2/SDL.h>

namespace
{
  typedef the::ctci::Factory< yarrr::Event > EventFactory;

  class Client
  {
    public:
      Client( the::net::Connection& connection )
        : connection( connection )
      {
      }

      the::net::Connection& connection;
  };

  class ConnectionEstablisher
  {
    public:
      ConnectionEstablisher( the::time::Clock& clock, const the::net::Address& address )
        : m_network_service(
          std::bind( &ConnectionEstablisher::new_connection, this, std::placeholders::_1 ),
          std::bind( &ConnectionEstablisher::lost_connection, this, std::placeholders::_1 ) )
        , m_clock( clock )
        , m_clock_synchronizer( nullptr )
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
        m_client.reset( new Client( connection ) );
      }

      void lost_connection( the::net::Connection& )
      {
        std::cout << "connection lost" << std::endl;
      }

    private:
      the::net::Service m_network_service;
      std::unique_ptr< Client > m_client;
      std::mutex m_client_mutex;
      the::time::Clock& m_clock;
      typedef yarrr::clock_sync::Client< the::time::Clock, the::net::Connection > ClockSync;
      typedef ClockSync* ClockSyncPointer;
      ClockSyncPointer m_clock_synchronizer;
  };

}

class DrawableShip : public DrawableObject
{
  public:
    DrawableShip( SdlEngine& graphics_engine )
      : DrawableObject( graphics_engine )
    {
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
      m_graphical_engine.draw_ship( m_local_ship );
    }

    void command( char cmd, const the::time::Time timestamp )
    {
      yarrr::travel_in_time_to( timestamp, m_local_ship );
      switch( cmd )
      {
        case 1: thruster(); break;
        case 2: ccw(); break;
        case 3: cw(); break;
      }
    }

  private:
    void thruster()
    {
      const yarrr::Coordinate heading{
        static_cast< int64_t >( 40.0 * cos( m_local_ship.angle * 3.14 / 180.0 / 4.0 ) ),
        static_cast< int64_t >( 40.0 * sin( m_local_ship.angle * 3.14 / 180.0 / 4.0 ) ) };
      m_local_ship.velocity += heading;
    }

    void ccw()
    {
      m_local_ship.vangle -= 100;
    }

    void cw()
    {
      m_local_ship.vangle += 100;
    }

    yarrr::Object m_local_ship;
    yarrr::Object m_network_ship;
};

int main( int argc, char ** argv )
{
  typedef std::map< int, std::unique_ptr< DrawableShip > > ShipContainer;
  ShipContainer ships;
  SdlEngine graphics_engine( 1024, 768 );

  EventFactory event_factory;
  the::ctci::ExactCreator< yarrr::Event, yarrr::LoginResponse > login_response_creator;
  event_factory.register_creator( yarrr::LoginResponse::ctci, login_response_creator );

  the::ctci::ExactCreator< yarrr::Event, yarrr::ObjectStateUpdate > object_state_creator;
  event_factory.register_creator( yarrr::ObjectStateUpdate::ctci, object_state_creator );

  the::ctci::Dispatcher event_dispatcher;
  event_dispatcher.register_listener<yarrr::LoginResponse>( yarrr::LoginResponse::ctci,
      []( const yarrr::LoginResponse& )
      {
        std::cout << "login response arrived" << std::endl;
      } );

  event_dispatcher.register_listener<yarrr::ObjectStateUpdate>( yarrr::ObjectStateUpdate::ctci,
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
        "localhost:2001") );
  Client& client( establisher.wait_for_connection() );


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
            cmd = 1;
            break;
          case SDLK_LEFT:
            cmd = 2;
            break;
          case SDLK_RIGHT:
            cmd = 3;
            break;
        }

        client.connection.send( yarrr::Command( cmd, now ).serialize() );
        begin( ships )->second->command( cmd, now );
      }
    }

    the::net::Data message;
    while ( client.connection.receive( message ) )
    {
      yarrr::Event::Pointer event( event_factory.create( yarrr::extract<the::ctci::Id>( &message[0] ) ) );
      if ( !event )
      {
        continue;
      }

      event->deserialize( message );
      event_dispatcher.dispatch( event->polymorphic_ctci(), *event );
    }

    for ( auto& ship : ships )
    {
      ship.second->travel_in_time_to( now );
    }

    graphics_engine.update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

