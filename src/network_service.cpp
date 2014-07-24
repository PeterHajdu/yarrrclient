#include "network_service.hpp"
#include <thenet/address.hpp>
#include <thectci/dispatcher.hpp>
#include <yarrr/command.hpp>
#include <yarrr/login.hpp>
//todo: replace with decent log framework
#include <iostream>

extern the::ctci::Dispatcher local_event_dispatcher;

NetworkService::NetworkService(
    the::time::Clock& clock,
    const the::net::Address& address )
  : m_network_service(
      std::bind( &NetworkService::new_connection, this, std::placeholders::_1 ),
      std::bind( &NetworkService::lost_connection, this, std::placeholders::_1 ) )
  , m_clock( clock )
{
  std::cout << "connecting to host: " << address.host << ", port: " << address.port << std::endl;
  m_network_service.connect_to( address );
  m_network_service.start();

  local_event_dispatcher.register_listener<yarrr::Command>(
      std::bind( &NetworkService::handle_command, this, std::placeholders::_1 ) );
}

void
NetworkService::handle_command( const yarrr::Command& command )
{
  //todo: somehow these locks should be avoided
  std::lock_guard< std::mutex > connection_guard( m_connection_mutex );
  if ( !m_connection_wrapper )
  {
    return;
  }

  m_connection_wrapper->connection.send( command.serialize() );
}

void
NetworkService::send( yarrr::Data&& data )
{
  std::lock_guard< std::mutex > connection_guard( m_connection_mutex );
  if ( !m_connection_wrapper )
  {
    return;
  }

  m_connection_wrapper->connection.send( std::move( data ) );
}

void
NetworkService::process_incoming_messages()
{
  std::lock_guard< std::mutex > connection_guard( m_connection_mutex );
  if ( !m_connection_wrapper )
  {
    return;
  }

  m_connection_wrapper->process_incoming_messages();
}

void
NetworkService::new_connection( the::net::Connection& connection )
{
  connection.register_task( std::unique_ptr< ClockSync >( new ClockSync( m_clock, connection ) ) );

  std::lock_guard< std::mutex > connection_guard( m_connection_mutex );
  std::cout << "new connection established" << std::endl;
  m_connection_wrapper.reset( new ConnectionWrapper( connection ) );
  local_event_dispatcher.dispatch( ConnectionEstablished( *m_connection_wrapper ) );
}

void
NetworkService::lost_connection( the::net::Connection& )
{
  std::cout << "connection lost" << std::endl;
  //todo: try to reconnect
  exit( 1 );
}


LoginHandler::LoginHandler()
{
  m_dispatcher.register_listener< yarrr::LoginResponse >(
      std::bind( &LoginHandler::handle_login_response, this, std::placeholders::_1 ) );
  local_event_dispatcher.register_listener<ConnectionEstablished>(
      std::bind( &LoginHandler::handle_connection_established, this, std::placeholders::_1 ) );
}

void
LoginHandler::handle_connection_established( const ConnectionEstablished& connection_established )
{
  connection_established.connection_wrapper.register_dispatcher( m_dispatcher );
  log_in( connection_established.connection_wrapper );
}

void
LoginHandler::log_in( ConnectionWrapper& connection_wrapper )
{
  connection_wrapper.connection.send( yarrr::LoginRequest( "appletree" ).serialize() );
}

void
LoginHandler::handle_login_response( const yarrr::LoginResponse& response )
{
  local_event_dispatcher.dispatch( LoggedIn( response.object_id() ) );
}

