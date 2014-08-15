#include "network_service.hpp"
#include "keyboard_handler.hpp"
#include "local_event_dispatcher.hpp"
#include "world.hpp"
#include "fps.hpp"
#include "particle_factory.hpp"

#include <yarrr/graphical_engine.hpp>
#include <yarrr/stream_to_chat.hpp>
#include <yarrr/object_container.hpp>
#include <yarrr/basic_behaviors.hpp>
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
  the::log::Logger::add_channel( stream_to_chat.stream() );
  the::conf::set_value( "login_name", getenv( "LOGNAME" ) );
  the::time::Clock clock;

  FpsDrawer fps_drawer( clock );

  yarrr::ObjectContainer object_container;
  World world( object_container );

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
  //todo: this is ugly, a separate log class should be used
  stream_to_chat.register_dispatcher( the::ctci::service<LocalEventDispatcher>().incoming );

  yarrr::ParticleContainer particles;
  the::ctci::AutoServiceRegister< yarrr::ParticleFactory, ParticleFactory >
    auto_particle_factory_register( particles );

  while ( running )
  {
    the::time::Clock::Time now( clock.now() );

    keyboard_handler.check_keyboard( now );
    network_service.process_incoming_messages();

    stream_to_chat.flush();

    object_container.dispatch( yarrr::TimerUpdate( now ) );
    particles.travel_in_time_to( now );

    world.in_focus();

    the::ctci::service<yarrr::GraphicalEngine>().update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

