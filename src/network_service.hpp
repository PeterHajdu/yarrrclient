#pragma once
#include <thectci/id.hpp>
#include <thenet/service.hpp>
#include <thenet/address.hpp>
#include <yarrr/physical_parameters.hpp>
#include <yarrr/connection_wrapper.hpp>
#include <yarrr/clock_synchronizer.hpp>

typedef yarrr::ConnectionWrapper< the::net::Connection > ConnectionWrapper;

class LoggedIn
{
  public:
    add_ctci( "logged_id" );
    LoggedIn( yarrr::PhysicalParameters::Id user_id )
      : user_id( user_id )
    {
    }

    const yarrr::PhysicalParameters::Id user_id;
};

//todo: hide this the important one is logged in
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

namespace yarrr
{
  class LoginResponse;
}

class LoginHandler
{
  public:
    LoginHandler();
    void handle_connection_established( const ConnectionEstablished& connection_established );
    void handle_login_response( const yarrr::LoginResponse& response );
  private:
    the::ctci::Dispatcher m_dispatcher;
    the::ctci::Dispatcher& m_local_event_dispatcher;
};

namespace yarrr
{
  class Command;
}

class NetworkService
{
  public:
    NetworkService( the::time::Clock& clock, const the::net::Address& address );
    void process_incoming_messages();

  private:
    void handle_command( const yarrr::Command& command );
    void send( yarrr::Data&& data );
    void new_connection( the::net::Connection& connection );
    void lost_connection( the::net::Connection& );

    the::net::Service m_network_service;
    std::unique_ptr< ConnectionWrapper > m_connection_wrapper;
    std::mutex m_connection_mutex;
    the::time::Clock& m_clock;
    typedef yarrr::clock_sync::Client< the::time::Clock, the::net::Connection > ClockSync;
    LoginHandler m_login_handler;

    the::net::Address m_server_address;

    the::ctci::Dispatcher& m_local_event_dispatcher;
};

