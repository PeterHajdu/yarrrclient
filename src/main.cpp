#include "network_service.hpp"
#include "sdl_engine.hpp"
#include "keyboard_handler.hpp"
#include "local_event_dispatcher.hpp"
#include "world.hpp"

#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <string>
#include <map>

#include <yarrr/basic_behaviors.hpp>
#include <yarrr/object.hpp>
#include <yarrr/command.hpp>
#include <yarrr/object_state_update.hpp>
#include <yarrr/delete_object.hpp>

#include <thenet/address.hpp>
#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>
#include <thectci/dispatcher.hpp>
#include <thectci/service_registry.hpp>

namespace
{


}


int main( int argc, char ** argv )
{
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


  while ( running )
  {
    the::time::Clock::Time now( clock.now() );

    keyboard_handler.check_keyboard( now );
    network_service.process_incoming_messages();

    world.broadcast( yarrr::TimerUpdate( now ) );
    world.in_focus();

    the::ctci::service<SdlEngine>().update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

