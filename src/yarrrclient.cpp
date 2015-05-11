#include "network_service.hpp"
#include "keyboard_handler.hpp"
#include "local_event_dispatcher.hpp"
#include "world.hpp"
#include "fps.hpp"
#include "particle_factory.hpp"
#include "sdl_engine.hpp"
#include "mission_control.hpp"
#include "information_window.hpp"
#include "wakeup.hpp"
#include "authentication_token.hpp"

#include <yarrr/test_db.hpp>
#include <yarrr/id_generator.hpp>
#include <yarrr/modell.hpp>
#include <yarrr/resources.hpp>
#include <yarrr/graphical_engine.hpp>
#include <yarrr/dummy_graphical_engine.hpp>
#include <yarrr/object_container.hpp>
#include <yarrr/timer_update.hpp>
#include <themodel/zmq_remote.hpp>
#include <themodel/json_exporter.hpp>
#include <thenet/address.hpp>
#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>
#include <thectci/dispatcher.hpp>
#include <thectci/service_registry.hpp>
#include <theconf/configuration.hpp>

#include <yarrr/log.hpp>

#include <iostream>

#include <sys/stat.h>

namespace
{
std::unique_ptr< the::model::ZmqRemote >
create_remote_model_endpoint_if_needed()
{
  const auto remote_model_endpoint_key( "remote-model-endpoint" );
  if ( !the::conf::has( remote_model_endpoint_key ) )
  {
    return nullptr;
  }

  return std::make_unique< the::model::ZmqRemote >(
    the::conf::get<std::string>( remote_model_endpoint_key ).c_str(),
    yarrr::LuaEngine::model(),
    the::model::export_json );
}


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
  "  --username <username>\n"
  "  --auth_token <authentication token>\n"
  "  --loglevel <int >= 0>\n"
  << std::endl;
  exit( 0 );
}

void create_home_folder_if_needed()
{
  const std::string home_folder( std::string( getenv( "HOME" ) ) + "/.yarrr" );
  the::conf::set_value( "home_folder", home_folder );
  thelog( yarrr::log::debug )( "Creating home folder:", home_folder );
  mkdir( home_folder.c_str(), S_IRWXU );
}

void parse_and_handle_configuration( const the::conf::ParameterVector& parameters )
{
  create_home_folder_if_needed();
  the::conf::parse( parameters );

  if ( !the::conf::has( "username" ) )
  {
    the::conf::set_value( "username", getenv( "LOGNAME" ) );
  }

  if ( !the::conf::has( "auth_token" ) )
  {
    the::conf::set_value(
        "auth_token",
        yarrrc::load_authentication_token( the::conf::get_value( "username" ) ) );
  }

  if ( the::conf::has( "help" ) )
  {
    print_help_and_exit();
  }

  if ( !the::conf::has( "server" ) )
  {
    the::conf::set_value( "server", "test.yarrrthegame.com:2001" );
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

  yarrrc::NetworkService network_service(
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
  yarrrc::InformationWindow information_window( *graphical_engine );

  yarrrc::WakeupSender< the::time::Clock > wakeup_sender(
      clock,
      the::ctci::service<LocalEventDispatcher>().wakeup );

  the::ctci::AutoServiceRegister< yarrr::Db, test::Db > db;
  yarrr::IdGenerator id_generator;
  the::ctci::AutoServiceRegister< yarrr::ModellContainer, yarrr::ModellContainer > modell_container(
      yarrr::LuaEngine::model(),
      id_generator,
      db.get() );

  std::unique_ptr< the::model::ZmqRemote > remote_model_access( create_remote_model_endpoint_if_needed() );

  while ( running )
  {
    const the::time::Clock::Time now( clock.now() );

    keyboard_handler.check_keyboard( now );
    network_service.process_incoming_messages();

    object_container.dispatch( yarrr::TimerUpdate( now ) );
    particles.travel_in_time_to( now );

    wakeup_sender.tick();

    world.in_focus();

    the::ctci::service<yarrr::GraphicalEngine>().update_screen();
    frequency_stabilizer.stabilize();
    if ( remote_model_access )
    {
      remote_model_access->handle_requests();
    }
  }

  return 0;
}

