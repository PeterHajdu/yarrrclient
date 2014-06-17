#include <iostream>
#include <thread>
#include <memory>

#include <yarrr/ship.hpp>
#include <thenet/service.hpp>


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

int main( int argc, char ** argv )
{
  ConnectionEstablisher establisher;
  Client& client( establisher.wait_for_connection() );

  while ( true )
  {
    the::net::Data message;
    while ( client.connection.receive( message ) )
    {
      std::cout << "message from server " << std::string( &message[0], message.size() ) << std::endl;
    }
    std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
  }

  return 0;
}

