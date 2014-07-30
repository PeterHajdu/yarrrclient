#include "network_service.hpp"
#include "keyboard_handler.hpp"
#include "local_event_dispatcher.hpp"
#include "world.hpp"

#include <yarrr/graphical_engine.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/stream_to_chat.hpp>
#include <thenet/address.hpp>
#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>
#include <thectci/dispatcher.hpp>
#include <thectci/service_registry.hpp>
#include <theconf/configuration.hpp>

#include <thelog/logger.hpp>
#include <iostream>

int main( int argc, char ** argv )
{
  yarrr::StreamToChat stream_to_chat( "system" );
  the::log::Logger::add_channel( std::cout );
  the::log::Logger::add_channel( stream_to_chat.stream() );
  the::conf::set_value( "login_name", getenv( "LOGNAME" ) );
  the::time::Clock clock;

  World world;

  NetworkService network_service(
      clock,
      the::net::Address(
        argc > 1 ?
        argv[1] :
        "localhost:2001") );

  the::time::FrequencyStabilizer< 60, the::time::Clock > frequency_stabilizer( clock );

  bool running( true );
  KeyboardHandler keyboard_handler( running );
  keyboard_handler.register_dispatcher( the::ctci::service<LocalEventDispatcher>().dispatcher );
  stream_to_chat.register_dispatcher( the::ctci::service<LocalEventDispatcher>().dispatcher );

  while ( running )
  {
    the::time::Clock::Time now( clock.now() );

    keyboard_handler.check_keyboard( now );
    network_service.process_incoming_messages();

    stream_to_chat.flush();

    world.broadcast( yarrr::TimerUpdate( now ) );
    world.in_focus();

    the::ctci::service<yarrr::GraphicalEngine>().update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

