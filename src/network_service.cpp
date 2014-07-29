#include "network_service.hpp"
#include "local_event_dispatcher.hpp"
#include <thenet/address.hpp>
#include <thectci/dispatcher.hpp>
#include <yarrr/command.hpp>
#include <yarrr/login.hpp>
//todo: replace with decent log framework
#include <iostream>

NetworkService::NetworkService(
    the::time::Clock& clock,
    const the::net::Address& address )
  : m_network_service(
      std::bind( &NetworkService::new_connection, this, std::placeholders::_1 ),
      std::bind( &NetworkService::lost_connection, this, std::placeholders::_1 ) )
  , m_clock( clock )
  , m_server_address( address )
  , m_local_event_dispatcher( the::ctci::service< LocalEventDispatcher >().dispatcher )
{
  std::cout << "connecting to host: " << address.host << ", port: " << address.port << std::endl;
  m_network_service.connect_to( address );
  m_network_service.start();

  m_local_event_dispatcher.register_listener<yarrr::Command>(
      std::bind( &NetworkService::handle_command, this, std::placeholders::_1 ) );
}

void
NetworkService::handle_command( const yarrr::Command& command )
{
  send( command.serialize() );
}

void
NetworkService::send( yarrr::Data&& data )
{
  //todo: somehow these locks should be avoided
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
  //todo: move to main thread
  m_local_event_dispatcher.dispatch( ConnectionEstablished( *m_connection_wrapper ) );
}

void
NetworkService::lost_connection( the::net::Connection& )
{
  std::cout << "connection lost" << std::endl;
  //todo: try to reconnect
}


LoginHandler::LoginHandler()
  : m_local_event_dispatcher( the::ctci::service< LocalEventDispatcher >().dispatcher )
{
  m_dispatcher.register_listener< yarrr::LoginResponse >(
      std::bind( &LoginHandler::handle_login_response, this, std::placeholders::_1 ) );
  m_local_event_dispatcher.register_listener<ConnectionEstablished>(
      std::bind( &LoginHandler::handle_connection_established, this, std::placeholders::_1 ) );
}

void
LoginHandler::handle_connection_established( const ConnectionEstablished& connection_established )
{
  connection_established.connection_wrapper.register_dispatcher( m_dispatcher );
  connection_established.connection_wrapper.connection.send( yarrr::LoginRequest( "appletree" ).serialize() );
}

void
LoginHandler::handle_login_response( const yarrr::LoginResponse& response )
{
  m_local_event_dispatcher.dispatch( LoggedIn( response.object_id() ) );
}
