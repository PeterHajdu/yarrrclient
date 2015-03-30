#include "login_handler.hpp"
#include "local_event_dispatcher.hpp"
#include "authentication_token.hpp"
#include <theconf/configuration.hpp>
#include <yarrr/command.hpp>
#include <yarrr/log.hpp>
#include <yarrr/protocol.hpp>
#include <yarrr/crypto.hpp>

namespace yarrrc
{

LoginHandler::LoginHandler( the::ctci::Dispatcher& dispatcher )
  : m_dispatcher( dispatcher )
  , m_connection_established_listener( dispatcher.register_smart_listener< yarrrc::ConnectionEstablished >(
        [ this ]( const yarrrc::ConnectionEstablished& event )
        {
          handle_connection_established( event );
        } ) )
  , m_auth_request_callback()
  , m_connection( nullptr )
{
}

void
LoginHandler::handle_connection_established( const yarrrc::ConnectionEstablished& connection_established )
{
  thelog( yarrr::log::debug )( "Connection established." );
  m_connection = connection_established.connection_wrapper.connection;
  m_auth_request_callback = connection_established.connection_wrapper.register_smart_listener< yarrr::Command >(
      [ this ]( const yarrr::Command& command )
      {
        if ( command.command() != yarrr::Protocol::authentication_request )
        {
          return;
        }

        handle_authentication_request( command );
      } );

  const bool should_register( the::conf::get< std::string >( "auth_token" ) == "" );
  if ( should_register )
  {
    send_registration_request();
    return;
  }

  send_login_request();
}

void
LoginHandler::send_registration_request() const
{
  thelog( yarrr::log::debug )( "Sending registration request." );
  const std::string authentication_token( yarrr::auth_hash( yarrr::random( 256 ) ) );
  const std::string username( the::conf::get_value( "username" ) );
  m_connection->send(
      yarrr::Command( {
        yarrr::Protocol::registration_request,
        username,
        authentication_token } ).serialize() );

  save_authentication_token( username, authentication_token );
}

void
LoginHandler::send_login_request() const
{
  thelog( yarrr::log::debug )( "Sending login request." );
  m_connection->send(
      yarrr::Command( {
        yarrr::Protocol::login_request,
        the::conf::get_value( "username" ) } ).serialize() );
}

void
LoginHandler::handle_authentication_request( const yarrr::Command& command ) const
{
  thelog( yarrr::log::debug )( "Sending authentication response." );
  const auto& authentication_nonce( command.parameters()[ 0 ] );
  m_connection->send(
      yarrr::Command( {
        yarrr::Protocol::authentication_response,
        yarrr::auth_hash( authentication_nonce + the::conf::get_value( "auth_token" ) ) } ).serialize() );
}

}

