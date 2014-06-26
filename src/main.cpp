#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <string>

#include <yarrr/object.hpp>
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
      ConnectionEstablisher( const the::net::Address& address )
        : m_network_service(
          std::bind( &ConnectionEstablisher::new_connection, this, std::placeholders::_1 ),
          std::bind( &ConnectionEstablisher::lost_connection, this, std::placeholders::_1 ) )
      {
        std::cout << "connecting to host: " << address.host << ", port: " << address.port << std::endl;
        m_network_service.connect_to( address );
        m_network_service.start();
      }


      Client& wait_for_connection()
      {
        while ( !m_client )
        {
          std::lock_guard< std::mutex > connection_guard( m_client_mutex );
          std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }

        return *m_client;
      }

      void new_connection( the::net::Connection& connection )
      {
        std::lock_guard< std::mutex > connection_guard( m_client_mutex );
        std::cout << "new connection established" << std::endl;
        const std::string helloMessage( "hello world" );
        connection.send( the::net::Data( begin( helloMessage ), end( helloMessage ) ) );
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
  ConnectionEstablisher establisher( the::net::Address(
        argc > 1 ?
        argv[1] :
        "localhost:2001") );
  Client& client( establisher.wait_for_connection() );
  SdlEngine graphics_engine( 1024, 768 );

  typedef std::unordered_map< int, std::unique_ptr< DrawableShip > > ShipContainer;
  ShipContainer ships;

  the::time::Clock clock;
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
    }

    the::net::Data message;
    while ( client.connection.receive( message ) )
    {
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

