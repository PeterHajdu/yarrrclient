#pragma once

#include <yarrr/connection_wrapper.hpp>
#include <thenet/connection.hpp>

namespace yarrr
{
  class Command;
}

namespace yarrrc
{

using ConnectionWrapper = yarrr::ConnectionWrapper< the::net::Connection >;

class ConnectionEstablished
{
  public:
    add_ctci( "connection_established" );
    ConnectionEstablished( ConnectionWrapper& connection_wrapper )
      : connection_wrapper( connection_wrapper )
    {
    }

    ConnectionWrapper& connection_wrapper;
};

class LoginHandler
{
  public:
    LoginHandler( the::ctci::Dispatcher& );

  private:
    void handle_connection_established( const yarrrc::ConnectionEstablished& connection_established );
    void handle_authentication_request( const yarrr::Command& ) const;
    void send_registration_request() const;
    void send_login_request() const;

    the::ctci::Dispatcher& m_dispatcher;
    the::ctci::SmartListener m_connection_established_listener;
    the::ctci::SmartListener m_auth_request_callback;
    the::net::Connection::Pointer m_connection;
};

}

