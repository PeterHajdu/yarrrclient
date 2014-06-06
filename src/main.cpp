#include <iostream>
#include <array>

#include <yarrr/ship.hpp>
#include <yarrr/network.hpp>

namespace
{
  void new_connection( yarrr::Socket& )
  {
    std::cout << "new connection established" << std::endl;
  }

  void data_available_on( yarrr::Socket& socket, char* message, size_t length )
  {
    std::cout << "data arrived: " << std::string( message, length ) << std::endl;
  }
}

int main( int argc, char ** argv )
{
  yarrr::SocketPool pool(
      new_connection,
      data_available_on
      );
  if ( pool.connect( "localhost", 2000 ) )
  {
    pool.start();
  }
  return 0;
}

