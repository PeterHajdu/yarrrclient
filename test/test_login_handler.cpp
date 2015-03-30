#include "../src/login_handler.hpp"
#include <yarrr/test_connection.hpp>
#include <yarrr/command.hpp>
#include <yarrr/protocol.hpp>
#include <yarrr/crypto.hpp>
#include <theconf/configuration.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;


Describe(a_login_handler)
{
  void establish_connection()
  {
    connection = std::make_unique< test::Connection >();
    dispatcher.dispatch( yarrrc::ConnectionEstablished( connection->wrapper ) );
  }

  void set_up_username_without_authentication_token()
  {
    the::conf::set( "username", username );
    the::conf::set( "auth_token", "" );
  }

  void set_up_username_with_authentication_token()
  {
    the::conf::set( "username", username );
    the::conf::set( "auth_token", auth_token );
  }

  void SetUp()
  {
    login_handler.reset();
    login_handler = std::make_unique< yarrrc::LoginHandler >( dispatcher );
  }

  It( sends_out_registration_request_if_no_auth_token_file_is_found )
  {
    set_up_username_without_authentication_token();
    establish_connection();
    AssertThat( connection->has_entity< yarrr::Command >(), Equals( true ) );

    auto command( connection->get_entity< yarrr::Command >() );
    AssertThat( command->command(), Equals( yarrr::Protocol::registration_request ) );
    AssertThat( command->parameters(), HasLength( 2u ) );
    AssertThat( command->parameters()[ 0 ], Equals( username ) );
    std::cout << "Generated authentication token is: " << command->parameters()[ 1 ] << std::endl;
  }

  It( sends_out_login_request_if_auth_token_is_found )
  {
    set_up_username_with_authentication_token();
    establish_connection();
    AssertThat( connection->has_entity< yarrr::Command >(), Equals( true ) );

    auto command( connection->get_entity< yarrr::Command >() );
    AssertThat( command->command(), Equals( yarrr::Protocol::login_request ) );
    AssertThat( command->parameters(), !IsEmpty() );
    AssertThat( command->parameters()[ 0 ], Equals( username ) );
  }

  It( sends_out_authentication_response_to_authentication_request )
  {
    set_up_username_with_authentication_token();
    establish_connection();
    connection->flush_connection();
    connection->wrapper.dispatch( yarrr::Command( {
          yarrr::Protocol::authentication_request,
          auth_nonce } ) );
    AssertThat( connection->has_entity< yarrr::Command >(), Equals( true ) );
    auto command( connection->get_entity< yarrr::Command >() );
    AssertThat( command->command(), Equals( yarrr::Protocol::authentication_response ) );
    AssertThat( command->parameters(), !IsEmpty() );
    const std::string expected_auth_response( yarrr::auth_hash( auth_nonce + auth_token ) );
    std::cout << "auth nonce: " << auth_nonce << std::endl;
    std::cout << "auth token: " << auth_token << std::endl;
    std::cout << "expected auth response: " << expected_auth_response << std::endl;
    AssertThat( command->parameters()[ 0 ], Equals( expected_auth_response ) );
  }

  the::ctci::Dispatcher dispatcher;
  std::unique_ptr< test::Connection > connection;
  std::unique_ptr< yarrrc::LoginHandler > login_handler;
  const std::string username{ "Kilgor Trout" };
  const std::string auth_token{ yarrr::auth_hash( yarrr::random( 256 ) ) };
  const std::string auth_nonce{ yarrr::auth_hash( yarrr::random( 256 ) ) };
};

