#include <iostream>
#include "sdl_engine.hpp"
#include "world.hpp"
#include "keyboard_handler.hpp"
#include "particle_factory.hpp"
#include "local_event_dispatcher.hpp"
#include "network_service.hpp"
#include "mission_control.hpp"

#include <yarrr/log.hpp>
#include <yarrr/login.hpp>
#include <yarrr/chat_message.hpp>
#include <yarrr/lua_setup.hpp>
#include <yarrr/object_container.hpp>
#include <yarrr/object_exporter.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/resources.hpp>
#include <yarrr/object_factory.hpp>
#include <yarrr/command.hpp>
#include <yarrr/object_creator.hpp>
#include <yarrr/delete_object.hpp>
#include <yarrr/engine_dispatcher.hpp>
#include <yarrr/mission.hpp>
#include <yarrr/mission_factory.hpp>
#include <yarrr/mission_exporter.hpp>
#include <yarrr/main_thread_callback_queue.hpp>
#include <yarrr/graphical_engine.hpp>

#include <thetime/frequency_stabilizer.hpp>
#include <thetime/clock.hpp>
#include <thetime/once_in.hpp>
#include <thectci/service_registry.hpp>
#include <theconf/configuration.hpp>

namespace
{
the::ctci::AutoServiceRegister< yarrr::MissionFactory, yarrr::MissionFactory > mission_factory_register;
the::ctci::AutoServiceRegister< yarrr::ObjectFactory, yarrr::ObjectFactory > object_factory_register;
the::ctci::AutoServiceRegister< LocalEventDispatcher, LocalEventDispatcher > local_event_dispatcher_register;
the::ctci::AutoServiceRegister< yarrr::ResourceFinder, yarrr::ResourceFinder > resource_finder_register(
    yarrr::ResourceFinder::PathList{
    "/usr/local/share/yarrr/",
    "/usr/share/yarrr/" } );


the::ctci::AutoServiceRegister< yarrr::MainThreadCallbackQueue, yarrr::MainThreadCallbackQueue > auto_main_thread_callback_queue_register;

void print_help_and_exit()
{
std::cout <<
  "yarrrshipyard is a simulator. You can try out your ship designs before sending them in to be merged.\n"
  "To learn more about it please visit: http://yarrrthegame.com\n"
  "Or write an e-mail to: info@yarrrthegame.com\n"
  "\n"
  "usage: yarrrshipyard --config-path <lua configuration path>\n"
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
       !the::conf::has( "config-path" ) )
  {
    print_help_and_exit();
  }

  the::conf::set_value( "lua_configuration_path", the::conf::get_value( "config-path" ) + "/" );

  const int loglevel(
      the::conf::has( "loglevel" ) ?
      the::conf::get<int>( "loglevel" ) :
      yarrr::log::info );

  the::log::Logger::set_loglevel( loglevel );
  thelog( yarrr::log::debug )( "Loglevel is set to", loglevel );
}

yarrr::MissionsModel mission_exporter( "missions", yarrr::LuaEngine::model() );

void mission_finished( const yarrrc::MissionFinished& finished )
{
  mission_exporter.delete_node( std::to_string( finished.mission.id() ) );
}

std::string character_object_id;

void mission_requested( const std::string& name )
{
  yarrr::Mission::Pointer new_mission( the::ctci::service< yarrr::MissionFactory >().create_a( name ) );
  if ( !new_mission )
  {
    thelog( yarrr::log::info )( "Wasn't able to create mission with name:", name );
    return;
  }

  mission_exporter.add_node( the::model::OwningNode::Pointer( new yarrr::MissionModel(
          std::to_string( new_mission->id() ),
          character_object_id,
          mission_exporter ) ) );

  yarrrc::local_dispatch( *new_mission );
}

void ship_requested( const std::string& type )
{
  yarrr::Object::Pointer new_ship( the::ctci::service< yarrr::ObjectFactory >().create_a( type ) );
  if ( !new_ship )
  {
    return;
  }

  character_object_id = std::to_string( new_ship->id() );
  the::ctci::Dispatcher& local_event_dispatcher( the::ctci::service<LocalEventDispatcher>().dispatcher );
  local_event_dispatcher.dispatch( yarrr::ObjectAssigned( new_ship->id() ) );
  local_event_dispatcher.polymorphic_dispatch( *new_ship->generate_update() );
}

void command_handler( const yarrr::Command& command )
{
  const std::string name( command.command() );
  if ( name == "ship" )
  {
    ship_requested( command.parameters().back() );
    return;
  }

  if ( name == "mission" )
  {
    mission_requested( command.parameters().back() );
    return;
  }
}

void print_help()
{
  the::ctci::Dispatcher& incoming_dispatcher( the::ctci::service<LocalEventDispatcher>().incoming );
  incoming_dispatcher.dispatch( yarrr::ChatMessage( "Welcome to the shipyard.", "system" ) );
  incoming_dispatcher.dispatch( yarrr::ChatMessage( "You can try out your new ships by typing /ship <shiptype> in the terminal.", "system" ) );
  incoming_dispatcher.dispatch( yarrr::ChatMessage( "You can try out your new missions by typing /mission <mission name> in the terminal.", "system" ) );
}

}


int main( int argc, char ** argv )
{
  parse_and_handle_configuration( the::conf::ParameterVector( argv, argv + argc ) );
  yarrr::initialize_lua_engine();

  std::unique_ptr< yarrr::GraphicalEngine > graphical_engine( new SdlEngine() );
  the::ctci::ServiceRegistry::register_service< yarrr::GraphicalEngine >( *graphical_engine );
  the::time::Clock clock;
  yarrr::ObjectContainer object_container;
  yarrrc::World world( object_container );
  yarrr::ObjectExporter object_exporter( object_container, yarrr::LuaEngine::model() );

  the::time::FrequencyStabilizer< 60, the::time::Clock > frequency_stabilizer( clock );

  the::ctci::Dispatcher& local_event_dispatcher( the::ctci::service<LocalEventDispatcher>().dispatcher );

  bool running( true );
  KeyboardHandler keyboard_handler( running );
  keyboard_handler.register_dispatcher( local_event_dispatcher );
  the::ctci::service<LocalEventDispatcher>().outgoing.register_dispatcher( local_event_dispatcher );

  yarrr::ParticleContainer particles;
  the::ctci::AutoServiceRegister< yarrr::ParticleFactory, ParticleFactory >
    auto_particle_factory_register( particles );

  ship_requested( "ship" );

  local_event_dispatcher.register_listener< yarrr::Command >( &command_handler );
  the::ctci::service<yarrr::EngineDispatcher>().register_listener< yarrr::ObjectCreated >(
      [ &object_container ]( const yarrr::ObjectCreated& event )
      {
        yarrr::Object* new_object( event.object.release() );
        the::ctci::service< yarrr::MainThreadCallbackQueue >().push_back(
            [ &object_container, new_object ]()
            {
              object_container.add_object( yarrr::Object::Pointer( new_object ) );
            }
        );
      } );

  the::ctci::service<yarrr::EngineDispatcher>().register_listener< yarrr::DeleteObject >(
      [ &object_container ]( const yarrr::DeleteObject & event )
      {
        const yarrr::Object::Id object_id( event.object_id() );
        the::ctci::service< yarrr::MainThreadCallbackQueue >().push_back(
            std::bind( &yarrr::ObjectContainer::delete_object, &object_container, object_id ) );
      } );

  print_help();
  yarrrc::MissionControl mission_control( *graphical_engine, local_event_dispatcher );
  local_event_dispatcher.register_listener< yarrrc::MissionFinished >( &mission_finished );

  the::time::OnceIn< the::time::Clock > mission_updater( clock, the::time::Clock::ticks_per_second );
  while ( running )
  {
    const the::time::Clock::Time now( clock.now() );

    keyboard_handler.check_keyboard( now );

    object_container.dispatch( yarrr::TimerUpdate( now ) );
    particles.travel_in_time_to( now );
    object_exporter.refresh();

    mission_updater.run(
        [ &mission_control ]()
        {
          mission_control.update();
        } );

    world.in_focus();

    the::ctci::service<yarrr::GraphicalEngine>().update_screen();
    frequency_stabilizer.stabilize();

    the::ctci::service< yarrr::MainThreadCallbackQueue >().process_callbacks();
  }

  return 0;
}

