#include <iostream>
#include <array>

#include <yarrr/ship.hpp>
#include <thenet/socket_pool.hpp>

namespace
{
  void new_connection( the::net::Socket& socket )
  {
    std::cout << "new connection established" << std::endl;
    const std::string helloMessage( "hello world" );
    socket.send( helloMessage.data(), helloMessage.size() );
  }

  void lost_connection( the::net::Socket& )
  {
    std::cout << "connection lost" << std::endl;
  }

  void data_available_on( the::net::Socket& socket, const char* message, size_t length )
  {
    std::cout << "data arrived: " << std::string( message, length ) << std::endl;
  }
}

int main( int argc, char ** argv )
{
  the::net::SocketPool pool(
      new_connection,
      lost_connection,
      data_available_on );

  if ( pool.connect( "localhost", 2000 ) )
  {
    pool.start();
  }
  return 0;
}

