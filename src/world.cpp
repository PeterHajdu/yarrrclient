#include "world.hpp"

#include "network_service.hpp"
#include "sdl_engine.hpp"
#include "local_event_dispatcher.hpp"
#include <yarrr/delete_object.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/graphical_engine.hpp>
#include <yarrr/command.hpp>
#include <yarrr/object_container.hpp>
#include <thectci/service_registry.hpp>

World::World( yarrr::ObjectContainer& object_container )
  : m_objects( object_container )
  , m_my_ship_id( 0 )
  , m_my_ship( nullptr )
{
  m_dispatcher.register_listener<yarrr::ObjectUpdate>(
      std::bind( &World::handle_object_update, this, std::placeholders::_1 ) );
  m_dispatcher.register_listener<yarrr::DeleteObject>(
      std::bind( &World::handle_delete_object, this, std::placeholders::_1 ) );
  the::ctci::Dispatcher& local_event_dispatcher(
      the::ctci::service< LocalEventDispatcher >().dispatcher );
  local_event_dispatcher.register_listener< LoggedIn >(
      std::bind( &World::handle_login, this, std::placeholders::_1 ) );
  local_event_dispatcher.register_listener<ConnectionEstablished>(
      std::bind( &World::handle_connection_established, this, std::placeholders::_1 ) );
  local_event_dispatcher.register_listener<yarrr::Command>(
      std::bind( &World::handle_command, this, std::placeholders::_1 ) );
}

void
World::handle_connection_established( const ConnectionEstablished& connection_established )
{
  connection_established.connection_wrapper.register_dispatcher( m_dispatcher );
}

void
World::handle_login( const LoggedIn& login )
{
  m_my_ship_id = login.user_id;
}

void
World::in_focus()
{
  if ( !m_my_ship )
  {
    return;
  }

  m_my_ship->dispatcher.dispatch( yarrr::FocusOnObject() );
}

void
World::handle_command( const yarrr::Command& command )
{
  if ( !m_my_ship )
  {
    return;
  }

  m_my_ship->dispatcher.dispatch( command );
}

void
World::handle_delete_object( const yarrr::DeleteObject& delete_object )
{
  m_objects.delete_object( delete_object.object_id() );
}


void
World::handle_object_update( const yarrr::ObjectUpdate& update )
{
  if ( m_objects.has_object_with_id( update.id() ) )
  {
    m_objects.handle_object_update( update );
    return;
  }

  yarrr::Object::Pointer new_object( update.create_object() );

  if ( update.id() == m_my_ship_id )
  {
    m_my_ship = new_object.get();
    m_hud.reset( new yarrr::Hud(
          the::ctci::service< yarrr::GraphicalEngine >(),
          m_my_ship->components.component< yarrr::PhysicalBehavior >().physical_parameters ) );
  }

  m_objects.add_object( std::move( new_object ) );
}

