#include "world.hpp"

#include "network_service.hpp"
#include "sdl_engine.hpp"
#include "local_event_dispatcher.hpp"
#include <yarrr/delete_object.hpp>
#include <yarrr/object_state_update.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/graphical_engine.hpp>
#include <yarrr/command.hpp>
#include <yarrr/object_container.hpp>
#include <thectci/service_registry.hpp>

namespace
{
  class FocusOnObject { public: add_ctci( "focus_on_object" ) };

  class GraphicalBehavior :
    public yarrr::ObjectBehavior,
    public yarrr::GraphicalObject
  {
    public:
      GraphicalBehavior()
        : yarrr::GraphicalObject( the::ctci::service< yarrr::GraphicalEngine >() )
        , m_local_physical_behavior( nullptr )
      {
      }

      void register_to( the::ctci::Dispatcher& dispatcher, the::ctci::ComponentRegistry& registry )
      {
        m_local_physical_behavior = &registry.component< yarrr::LocalPhysicalBehavior >();
        dispatcher.register_listener< FocusOnObject >( std::bind(
              &GraphicalBehavior::handle_focus_on_object, this, std::placeholders::_1 ) );
      }

      void handle_focus_on_object( const FocusOnObject& )
      {
        assert( m_local_physical_behavior );
        m_graphical_engine.focus_to( m_local_physical_behavior->physical_parameters.coordinate );
      }

      virtual void draw() const override
      {
        assert( m_local_physical_behavior );
        m_graphical_engine.draw_ship( m_local_physical_behavior->physical_parameters );
      }

    private:
      yarrr::LocalPhysicalBehavior* m_local_physical_behavior;
  };

  yarrr::Object::Pointer create_basic_ship()
  {
    yarrr::Object::Pointer ship( new yarrr::Object() );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::LocalPhysicalBehavior() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::SimplePhysicsUpdater() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::NetworkSynchronizer() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::Engine() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new GraphicalBehavior() ) );
    return ship;
  }
}


World::World( yarrr::ObjectContainer& object_container )
  : m_objects( object_container )
  , m_my_ship( nullptr )
{
  m_dispatcher.register_listener<yarrr::ObjectStateUpdate>(
      std::bind( &World::handle_object_state_update, this, std::placeholders::_1 ) );
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
  yarrr::Object::Pointer new_object( create_basic_ship() );
  m_my_ship = new_object.get();
  m_objects.add_object( login.user_id, std::move( new_object ) );
}

void
World::in_focus()
{
  if ( !m_my_ship )
  {
    return;
  }

  m_my_ship->dispatch( FocusOnObject() );
}

void
World::handle_command( const yarrr::Command& command )
{
  if ( !m_my_ship )
  {
    return;
  }

  m_my_ship->dispatch( command );
}

void
World::handle_delete_object( const yarrr::DeleteObject& delete_object )
{
  m_objects.delete_object( delete_object.object_id() );
}

void
World::handle_object_state_update( const yarrr::ObjectStateUpdate& object_state_update )
{
  const yarrr::PhysicalParameters& physical_parameters( object_state_update.physical_parameters() );
  const yarrr::Object::Id id( physical_parameters.id );

  if ( !m_objects.has_object_with_id( id ) )
  {
    yarrr::Object::Pointer new_object( create_basic_ship() );
    m_objects.add_object( id, std::move( new_object ) );
  }

  m_objects.object_with_id( id ).dispatch( object_state_update );
}

