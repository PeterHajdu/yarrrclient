#include <iostream>
#include <thread>
#include <memory>

#include <yarrr/ship.hpp>
#include <thenet/service.hpp>

#include "sdl_engine.hpp"

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
      ConnectionEstablisher()
        : m_network_service(
          std::bind( &ConnectionEstablisher::new_connection, this, std::placeholders::_1 ),
          std::bind( &ConnectionEstablisher::lost_connection, this, std::placeholders::_1 ) )
      {
        m_network_service.connect_to( "localhost", 2000 );
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

    void update_ship( the::net::Data data )
    {
      m_ship = yarrr::deserialize( std::string( begin( data ), end( data ) ) );
    }

    void draw() override
    {
      std::cout << "drawing ship " << yarrr::serialize( m_ship ) << std::endl;
      m_graphical_engine.draw_ship( m_ship );
    }

    yarrr::Ship m_ship;
};

int main( int argc, char ** argv )
{
  ConnectionEstablisher establisher;
  Client& client( establisher.wait_for_connection() );
  SdlEngine graphics_engine( 1024, 768 );

  DrawableShip serenity( graphics_engine );
  while ( true )
  {
    the::net::Data message;
    while ( client.connection.receive( message ) )
    {
      serenity.update_ship( message );
    }

    graphics_engine.update_screen();
    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
  }

  return 0;
}

