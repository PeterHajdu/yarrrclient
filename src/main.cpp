#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <string>

#include <yarrr/object.hpp>
#include <yarrr/clock_synchronizer.hpp>
#include <thenet/service.hpp>
#include <thenet/address.hpp>

#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>

#include "sdl_engine.hpp"
#include <SDL2/SDL.h>

namespace
{
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
      std::cout << "updating" << std::endl;
      m_ship = ship;
    }

    void advance_time_to( const the::time::Clock::Time& timestamp )
    {
      if ( m_ship.timestamp >= timestamp )
      {
        return;
      }

      std::cout << "approximating" << std::endl;
      yarrr::advance_time_to( timestamp, m_ship );
    }

    void draw() override
    {
      std::cout << "drawing ship " << yarrr::serialize( m_ship ) << std::endl;
      m_graphical_engine.draw_ship( m_ship );
    }

    yarrr::Object m_ship;
};

int main( int argc, char ** argv )
{
  the::time::Clock clock;
  ConnectionEstablisher establisher(
      clock,
      the::net::Address(
        argc > 1 ?
        argv[1] :
        "localhost:2001") );
  Client& client( establisher.wait_for_connection() );
  SdlEngine graphics_engine( 1024, 768 );

  typedef std::unordered_map< int, std::unique_ptr< DrawableShip > > ShipContainer;
  ShipContainer ships;

  the::time::FrequencyStabilizer< 60, the::time::Clock > frequency_stabilizer( clock );

  bool running( true );
  while ( running )
  {
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
         switch( event.key.keysym.sym )
         {
           case SDLK_q: running = false; break;
           case SDLK_UP: client.connection.send( the::net::Data( 1, 1 ) ); break;
           case SDLK_LEFT: client.connection.send( the::net::Data( 1, 2 ) ); break;
           case SDLK_RIGHT: client.connection.send( the::net::Data( 1, 3 ) ); break;
         }
      }
    }

    the::net::Data message;
    while ( client.connection.receive( message ) )
    {
      if ( message[ 0 ] == yarrr::clock_sync::protocol_id )
      {
        std::cout << "time sync message" << std::endl;
        continue;
      }

      yarrr::Object ship( yarrr::deserialize( std::string( begin( message ), end( message ) ) ) );
      ShipContainer::iterator drawable_ship( ships.find( ship.id ) );
      if ( drawable_ship == ships.end() )
      {
        ships.emplace( std::make_pair(
              ship.id,
              std::unique_ptr< DrawableShip >( new DrawableShip( graphics_engine ) ) ) );
      }

      ship.timestamp = now;
      ships[ ship.id ]->update_ship( ship );
    }

    for ( auto& ship : ships )
    {
      ship.second->advance_time_to( now );
    }

    graphics_engine.update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

