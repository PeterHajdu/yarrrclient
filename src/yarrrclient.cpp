#include "network_service.hpp"
#include "keyboard_handler.hpp"
#include "local_event_dispatcher.hpp"
#include "world.hpp"
#include "fps.hpp"
#include "particle_factory.hpp"
#include "sdl_engine.hpp"
#include "mission_control.hpp"
#include "information_window.hpp"

#include <yarrr/resources.hpp>
#include <yarrr/graphical_engine.hpp>
#include <yarrr/dummy_graphical_engine.hpp>
#include <yarrr/object_container.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <thenet/address.hpp>
#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>
#include <thectci/dispatcher.hpp>
#include <thectci/service_registry.hpp>
#include <theconf/configuration.hpp>

#include <yarrr/log.hpp>

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
  "  --loglevel <int >= 0>\n"
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

  const int loglevel(
      the::conf::has( "loglevel" ) ?
      the::conf::get<int>( "loglevel" ) :
      yarrr::log::info );

  the::log::Logger::set_loglevel( loglevel );
  thelog( yarrr::log::debug )( "Loglevel is set to", loglevel );

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

  the::time::Clock clock;

  yarrrc::FpsDrawer fps_drawer( clock );

  yarrr::ObjectContainer object_container;
  yarrrc::World world( object_container );

  NetworkService network_service(
      clock,
      the::net::Address( the::conf::get< std::string>( "server" ) ) );

  the::time::FrequencyStabilizer< 60, the::time::Clock > frequency_stabilizer( clock );

  bool running( true );
  KeyboardHandler keyboard_handler( running );
  keyboard_handler.register_dispatcher( the::ctci::service<LocalEventDispatcher>().dispatcher );

  yarrr::ParticleContainer particles;
  the::ctci::AutoServiceRegister< yarrr::ParticleFactory, ParticleFactory >
    auto_particle_factory_register( particles );

  yarrrc::MissionControl mission_control( *graphical_engine, the::ctci::service<LocalEventDispatcher>().incoming );
  yarrrc::InformationWindow information_window( *graphical_engine, clock );

  while ( running )
  {
    const the::time::Clock::Time now( clock.now() );

    keyboard_handler.check_keyboard( now );
    network_service.process_incoming_messages();

    object_container.dispatch( yarrr::TimerUpdate( now ) );
    particles.travel_in_time_to( now );

    world.in_focus();

    the::ctci::service<yarrr::GraphicalEngine>().update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

