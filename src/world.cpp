#include "world.hpp"

#include "network_service.hpp"
#include "sdl_engine.hpp"
#include "local_event_dispatcher.hpp"
#include <yarrr/delete_object.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/graphical_engine.hpp>
#include <yarrr/ship_control.hpp>
#include <yarrr/object_container.hpp>
#include <yarrr/log.hpp>
#include <yarrr/protocol.hpp>
#include <yarrr/command.hpp>
#include <thectci/service_registry.hpp>
#include <sstream>

namespace yarrrc
{

World::World( yarrr::ObjectContainer& object_container )
  : m_objects( object_container )
  , m_my_ship_id( 0 )
  , m_my_ship( nullptr )
{
  the::ctci::Dispatcher& incoming_dispatcher(
      the::ctci::service< LocalEventDispatcher >().incoming );

  incoming_dispatcher.register_listener<yarrr::BasicObjectUpdate>(
      std::bind( &World::handle_object_update, this, std::placeholders::_1 ) );
  incoming_dispatcher.register_listener<yarrr::ObjectInitializer>(
      std::bind( &World::handle_object_init, this, std::placeholders::_1 ) );
  incoming_dispatcher.register_listener<yarrr::DeleteObject>(
      std::bind( &World::handle_delete_object, this, std::placeholders::_1 ) );

  the::ctci::Dispatcher& local_event_dispatcher(
      the::ctci::service< LocalEventDispatcher >().dispatcher );

  auto command_handler( [ this ]( const yarrr::Command& command )
      {
        if ( command.command() != yarrr::Protocol::object_assigned )
        {
          return;
        }

        handle_object_assigned( command );
      } );
  local_event_dispatcher.register_listener< yarrr::Command >( command_handler );
  incoming_dispatcher.register_listener< yarrr::Command >( command_handler );

  local_event_dispatcher.register_listener<yarrr::ShipControl>(
      std::bind( &World::handle_command, this, std::placeholders::_1 ) );

  local_event_dispatcher.register_listener<yarrr::ObjectInitializer>(
      std::bind( &World::handle_object_init, this, std::placeholders::_1 ) );

}

void
World::handle_object_assigned( const yarrr::Command& login )
{
  thelog( yarrr::log::debug )( "Changing my ship id to", login.parameters().back() );

  std::stringstream stream( login.parameters().back() );
  yarrr::Object::Id object_id;
  stream >> object_id;

  m_my_ship_id = object_id;
  m_my_ship = nullptr;
  m_hud.reset();
}

void
World::in_focus()
{
  if ( !m_my_ship )
  {
    thelog( yarrr::log::trace )( "Unable to focus to non existing object." );
    return;
  }

  m_my_ship->dispatcher.dispatch( yarrr::FocusOnObject() );
}

void
World::handle_command( const yarrr::ShipControl& command )
{
  if ( !m_my_ship )
  {
    thelog( yarrr::log::trace )( "Unable to handle ship control, object does not exist." );
    return;
  }

  m_my_ship->dispatcher.dispatch( command );
}

void
World::handle_delete_object( const yarrr::DeleteObject& delete_object )
{
  thelog( yarrr::log::debug )( "Deleting object.", delete_object.object_id() );
  if ( delete_object.object_id() == m_my_ship_id )
  {
    thelog( yarrr::log::debug )( "Deleting focused object." );
    m_my_ship = nullptr;
    m_my_ship_id = 0;
    m_hud.reset();
  }

  m_objects.delete_object( delete_object.object_id() );
}


void
World::handle_object_update( const yarrr::ObjectUpdate& update )
{
  thelog( yarrr::log::insane )( "Object update for", update.id() );
  if ( !m_objects.has_object_with_id( update.id() ) )
  {
    thelog( yarrr::log::insane )( "Waiting for an object init before updating:", update.id() );
    return;
  }

  m_objects.handle_object_update( update );
}

void
World::handle_object_init( const yarrr::ObjectUpdate& update )
{
  thelog( yarrr::log::insane )( "Object init for", update.id() );
  if ( m_objects.has_object_with_id( update.id() ) )
  {
    thelog( yarrr::log::insane )( "Object exists." );
    handle_object_update( update );
    return;
  }

  yarrr::Object::Pointer new_object( update.create_object() );

  if ( update.id() == m_my_ship_id )
  {
    thelog( yarrr::log::debug )( "Creating new hud." );
    m_my_ship = new_object.get();
    m_hud.reset( new yarrrc::Hud( the::ctci::service< yarrr::GraphicalEngine >(), *m_my_ship ) );
  }

  m_objects.add_object( std::move( new_object ) );
}

}

