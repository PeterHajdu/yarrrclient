#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <map>

#include <yarrr/connection_wrapper.hpp>
#include <yarrr/object.hpp>
#include <yarrr/clock_synchronizer.hpp>
#include <yarrr/event.hpp>
#include <yarrr/login.hpp>
#include <yarrr/command.hpp>
#include <yarrr/ship_control.hpp>
#include <yarrr/object_state_update.hpp>
#include <yarrr/event_factory.hpp>
#include <yarrr/delete_object.hpp>

#include <thenet/service.hpp>
#include <thenet/address.hpp>

#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>

#include <thectci/dispatcher.hpp>

#include "sdl_engine.hpp"
#include <SDL2/SDL.h>

namespace
{
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

      void update_ship( const yarrr::PhysicalParameters& ship )
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
      }

      void draw() override
      {
        m_graphical_engine.draw_ship( m_local_ship );
      }

    private:

      yarrr::PhysicalParameters m_local_ship;
      yarrr::PhysicalParameters m_network_ship;
      yarrr::ShipControl m_local_ship_control;
  };

  SdlEngine graphics_engine( 800, 600 );

  typedef yarrr::ConnectionWrapper< the::net::Connection > ConnectionWrapper;

  class World
  {
    public:
      World(
          ConnectionWrapper& connection_wrapper,
          yarrr::PhysicalParameters::Id ship_id )
        : m_ship_id( ship_id )
        , m_my_ship( nullptr )
      {
        m_dispatcher.register_listener<yarrr::ObjectStateUpdate>(
            std::bind( &World::handle_object_state_update, this, std::placeholders::_1 ) );
        m_dispatcher.register_listener<yarrr::DeleteObject>(
            std::bind( &World::handle_delete_object, this, std::placeholders::_1 ) );
        connection_wrapper.register_dispatcher( m_dispatcher );
      }

      void in_focus()
      {
        if ( !m_my_ship )
        {
          return;
        }

        m_my_ship->in_focus();
      }

      void handle_command( const yarrr::Command& command )
      {
        if ( !m_my_ship )
        {
          return;
        }

        m_my_ship->handle_command( command );
      }

      void update_to( the::time::Time timestamp )
      {
        for ( auto& ship : m_ships )
        {
          ship.second->travel_in_time_to( timestamp );
        }
      }

      void handle_delete_object( const yarrr::DeleteObject& delete_object )
      {
        m_ships.erase( delete_object.object_id() );
      }

      void handle_object_state_update( const yarrr::ObjectStateUpdate& object_state_update )
      {
        const yarrr::PhysicalParameters& ship( object_state_update.physical_parameters() );
        ShipContainer::iterator drawable_ship( m_ships.find( ship.id ) );
        if ( drawable_ship == m_ships.end() )
        {
          m_ships.emplace( std::make_pair(
                ship.id,
                std::unique_ptr< DrawableShip >( new DrawableShip( graphics_engine ) ) ) );
        }

        if ( ship.id == m_ship_id )
        {
          //todo: remove lookup duplication
          m_my_ship = m_ships[ ship.id ].get();
        }

        m_ships[ ship.id ]->update_ship( ship );
      }

    private:
      typedef std::map< int, std::unique_ptr< DrawableShip > > ShipContainer;
      ShipContainer m_ships;
      the::ctci::Dispatcher m_dispatcher;
      yarrr::PhysicalParameters::Id m_ship_id;
      DrawableShip* m_my_ship;
  };

  class LoginHandler
  {
    public:
      LoginHandler( ConnectionWrapper& connection_wrapper )
        : m_connection_wrapper( connection_wrapper )
        , m_logged_in( false )
        , m_user_id( 0 )
      {
        m_dispatcher.register_listener< yarrr::LoginResponse >(
            std::bind( &LoginHandler::handle_login_response, this, std::placeholders::_1 ) );
        m_connection_wrapper.register_dispatcher( m_dispatcher );
      }

      void handle_incoming_messages()
      {
        m_connection_wrapper.process_incoming_messages();
      }

      void log_in()
      {
        m_connection_wrapper.connection.send( yarrr::LoginRequest( "appletree" ).serialize() );
        while ( !m_logged_in )
        {
          handle_incoming_messages();
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }
      }

      void handle_login_response( const yarrr::LoginResponse& response )
      {
        m_user_id = response.object_id();
        m_logged_in = true;
      }

      yarrr::PhysicalParameters::Id user_id() const
      {
        return m_user_id;
      }

    private:
      ConnectionWrapper& m_connection_wrapper;
      the::ctci::Dispatcher m_dispatcher;
      bool m_logged_in;
      yarrr::PhysicalParameters::Id m_user_id;
  };


  class ConnectionEstablisher
  {
    public:
      ConnectionEstablisher(
          the::time::Clock& clock,
          const the::net::Address& address )
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


      ConnectionWrapper& wait_for_connection()
      {
        while ( !m_connection_wrapper )
        {
          std::cout << "connecting..." << std::endl;
          std::lock_guard< std::mutex > connection_guard( m_connection_mutex );
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

        return *m_connection_wrapper;
      }

      void new_connection( the::net::Connection& connection )
      {
        std::unique_ptr< ClockSync > clock_synchronizer(
            new yarrr::clock_sync::Client< the::time::Clock, the::net::Connection >(
              m_clock,
              connection ) );
        m_clock_synchronizer = clock_synchronizer.get();
        connection.register_task( std::move( clock_synchronizer ) );

        std::lock_guard< std::mutex > connection_guard( m_connection_mutex );
        std::cout << "new connection established" << std::endl;
        m_connection_wrapper.reset( new ConnectionWrapper( connection ) );
      }

      void lost_connection( the::net::Connection& )
      {
        std::cout << "connection lost" << std::endl;
        exit( 1 );
      }

    private:
      the::net::Service m_network_service;
      std::unique_ptr< ConnectionWrapper > m_connection_wrapper;
      std::mutex m_connection_mutex;
      the::time::Clock& m_clock;
      typedef yarrr::clock_sync::Client< the::time::Clock, the::net::Connection > ClockSync;
      typedef ClockSync* ClockSyncPointer;
      ClockSyncPointer m_clock_synchronizer;
  };

}


int main( int argc, char ** argv )
{
  the::time::Clock clock;
  ConnectionEstablisher establisher(
      clock,
      the::net::Address(
        argc > 1 ?
        argv[1] :
        "localhost:2001") );
  ConnectionWrapper& network_connection( establisher.wait_for_connection() );
  LoginHandler login_handler( network_connection );
  login_handler.log_in();

  World world( network_connection, login_handler.user_id() );

  the::time::FrequencyStabilizer< 60, the::time::Clock > frequency_stabilizer( clock );

  bool running( true );
  while ( running )
  {
    network_connection.process_incoming_messages();
    the::time::Clock::Time now( clock.now() );
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
        world.handle_command( command );
        network_connection.connection.send( command.serialize() );
      }
    }

    network_connection.process_incoming_messages();

    world.update_to( now );
    world.in_focus();

    graphics_engine.update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

