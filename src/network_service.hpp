#pragma once
#include "login_handler.hpp"
#include <thectci/id.hpp>
#include <thenet/service.hpp>
#include <thenet/address.hpp>
#include <yarrr/object.hpp>
#include <yarrr/clock_synchronizer.hpp>
#include <yarrr/callback_queue.hpp>

namespace yarrr
{
  class ShipControl;
  class Command;
  class ChatMessage;
}

namespace the
{
namespace time
{
class Clock;
}
}


namespace yarrrc
{

class NetworkService
{
  public:
    NetworkService( the::time::Clock& clock, const the::net::Address& address );
    void process_incoming_messages();

  private:
    void handle_ship_control( const yarrr::ShipControl& command );
    void handle_chat_message( const yarrr::ChatMessage& chat_message );
    void handle_command( const yarrr::Command& command );
    void send( yarrr::Data&& data );
    void new_connection( the::net::Connection::Pointer connection );
    void new_connection_on_main_thread();

    void lost_connection( the::net::Connection::Pointer );

    the::net::Service m_network_service;
    std::unique_ptr< ConnectionWrapper > m_connection_wrapper;
    std::mutex m_connection_mutex;
    the::time::Clock& m_clock;
    typedef yarrr::clock_sync::Client< the::time::Clock, the::net::Connection > ClockSync;

    the::net::Address m_server_address;

    the::ctci::Dispatcher& m_local_event_dispatcher;
    the::ctci::Dispatcher& m_outgoing_dispatcher;
    yarrr::CallbackQueue m_callback_queue;
    LoginHandler m_login_handler;
};

}

