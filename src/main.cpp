#include "network_service.hpp"
#include "keyboard_handler.hpp"
#include "local_event_dispatcher.hpp"
#include "world.hpp"
#include "fps.hpp"
#include "particle_factory.hpp"
#include "sdl_engine.hpp"
#include "resources.hpp"

#include <yarrr/graphical_engine.hpp>
#include <yarrr/dummy_graphical_engine.hpp>
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

namespace
{

the::ctci::AutoServiceRegister< LocalEventDispatcher, LocalEventDispatcher > local_event_dispatcher_register;
the::ctci::AutoServiceRegister< yarrr::ResourceFinder, yarrr::ResourceFinder > resource_finder_register(
    yarrr::ResourceFinder::PathList{
    "/usr/local/share/yarrr/",
    "/usr/share/yarrr/" } );

void print_help_and_exit()
{
std::cout <<
  "yarrr is the most awesome spaceshooter game in the universe.\n"
  "To learn more about it please visit: http://yarrrthegame.com\n"
  "Or write an e-mail to: info@yarrrthegame.com\n"
  "\n"
  "usage: yarrrclient --server <server:port> [options]\n"
  "\n"
  "other options:\n"
  "  --fullscreen\n"
  << std::endl;
  exit( 0 );
}

void parse_and_handle_configuration( const the::conf::ParameterVector& parameters )
{
  the::conf::set_value( "login_name", getenv( "LOGNAME" ) );
  the::conf::parse( parameters );

  if ( the::conf::has( "help" ) ||
       !the::conf::has( "server" ) )
  {
    print_help_and_exit();
  }
}

typedef std::unique_ptr< yarrr::GraphicalEngine > GraphicalEnginePointer;
GraphicalEnginePointer create_graphical_engine()
{
  if ( the::conf::has( "text" ) )
  {
    return GraphicalEnginePointer( new yarrr::DummyGraphicalEngine() );
  }

  return GraphicalEnginePointer( new SdlEngine() );
}

}

int main( int argc, char ** argv )
{
  parse_and_handle_configuration( the::conf::ParameterVector( argv, argv + argc ) );
  GraphicalEnginePointer graphical_engine( create_graphical_engine() );
  the::ctci::ServiceRegistry::register_service< yarrr::GraphicalEngine >( *graphical_engine );

  yarrr::StreamToChat stream_to_chat( "system" );
  the::log::Logger::add_channel( stream_to_chat.stream() );
  the::time::Clock clock;

  FpsDrawer fps_drawer( clock );

  yarrr::ObjectContainer object_container;
  World world( object_container );

  NetworkService network_service(
      clock,
      the::net::Address( the::conf::get< std::string>( "server" ) ) );

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

