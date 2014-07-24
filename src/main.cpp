#include "network_service.hpp"
#include "sdl_engine.hpp"
#include <SDL2/SDL.h>

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
#include <thectci/multiplexer.hpp>


the::ctci::Dispatcher local_event_dispatcher;

namespace
{

  class FocusOnObject { public: add_ctci( "focus_on_object" ) };

  class GraphicalBehavior :
    public yarrr::ObjectBehavior,
    public DrawableObject
  {
    public:
      GraphicalBehavior( SdlEngine& graphics_engine )
        : DrawableObject( graphics_engine )
        , m_local_physical_behavior( nullptr )
        , m_graphical_engine( graphics_engine )
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

      virtual void draw() override
      {
        assert( m_local_physical_behavior );
        m_graphical_engine.draw_ship( m_local_physical_behavior->physical_parameters );
      }

    private:
      yarrr::LocalPhysicalBehavior* m_local_physical_behavior;
      SdlEngine& m_graphical_engine;
  };

  SdlEngine graphics_engine( 800, 600 );

  yarrr::Object::Pointer create_basic_ship()
  {
    yarrr::Object::Pointer ship( new yarrr::Object() );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::LocalPhysicalBehavior() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::SimplePhysicsUpdater() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::NetworkSynchronizer() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new yarrr::Engine() ) );
    ship->add_behavior( yarrr::ObjectBehavior::Pointer( new GraphicalBehavior( graphics_engine ) ) );
    return ship;
  }

  class World
  {
    public:
      World()
        : m_my_ship_id( 0 )
        , m_my_ship( nullptr )
      {
        m_dispatcher.register_listener<yarrr::ObjectStateUpdate>(
            std::bind( &World::handle_object_state_update, this, std::placeholders::_1 ) );
        m_dispatcher.register_listener<yarrr::DeleteObject>(
            std::bind( &World::handle_delete_object, this, std::placeholders::_1 ) );

        local_event_dispatcher.register_listener< LoggedIn >(
            std::bind( &World::handle_login, this, std::placeholders::_1 ) );
        local_event_dispatcher.register_listener<ConnectionEstablished>(
            std::bind( &World::handle_connection_established, this, std::placeholders::_1 ) );
        local_event_dispatcher.register_listener<yarrr::Command>(
            std::bind( &World::handle_command, this, std::placeholders::_1 ) );
      }

      void handle_connection_established( const ConnectionEstablished& connection_established )
      {
        connection_established.connection_wrapper.register_dispatcher( m_dispatcher );
      }

      void handle_login( const LoggedIn& login )
      {
        m_my_ship_id = login.user_id;
      }

      void in_focus()
      {
        if ( !m_my_ship )
        {
          return;
        }

        m_my_ship->dispatch( FocusOnObject() );
      }

      void handle_command( const yarrr::Command& command )
      {
        if ( !m_my_ship )
        {
          return;
        }

        m_my_ship->dispatch( command );
      }

      template < typename Event >
      void broadcast( const Event& event )
      {
        for ( auto& object : m_objects )
        {
          object.second->dispatch( event );
        }
      }

      void handle_delete_object( const yarrr::DeleteObject& delete_object )
      {
        m_objects.erase( delete_object.object_id() );
      }

      void handle_object_state_update( const yarrr::ObjectStateUpdate& object_state_update )
      {
        const yarrr::PhysicalParameters& physical_parameters( object_state_update.physical_parameters() );

        ObjectContainer::iterator object( m_objects.find( physical_parameters.id ) );
        if ( object == m_objects.end() )
        {
          object =
            m_objects.emplace( std::make_pair(
                  physical_parameters.id,
                  create_basic_ship() ) ).first;
        }

        if ( physical_parameters.id == m_my_ship_id )
        {
          m_my_ship = object->second.get();
        }

        object->second->dispatch( object_state_update );
      }

    private:
      typedef std::unordered_map< int, yarrr::Object::Pointer > ObjectContainer;
      ObjectContainer m_objects;
      the::ctci::Dispatcher m_dispatcher;
      yarrr::PhysicalParameters::Id m_my_ship_id;
      yarrr::Object* m_my_ship;
  };



}


class KeyboardHandler : public the::ctci::Multiplexer
{
  public:
    KeyboardHandler( bool& running )
      : m_running( running )
    {
    }

    void send_command( yarrr::Command::Type cmd, the::time::Time& timestamp )
    {
      yarrr::Command command( cmd, timestamp );
      dispatch( command );
    }

    void check_keyboard( the::time::Time& now )
    {
      SDL_PumpEvents();
      const unsigned char *keystates = SDL_GetKeyboardState( nullptr );
      if (keystates[SDL_SCANCODE_RIGHT])
      {
        send_command( yarrr::Command::cw, now );
      }

      if (keystates[SDL_SCANCODE_LEFT])
      {
        send_command( yarrr::Command::ccw, now );
      }

      if (keystates[SDL_SCANCODE_UP])
      {
        send_command( yarrr::Command::thruster, now );
      }

      if (keystates[SDL_SCANCODE_Q])
      {
        m_running = false;
      }
    }

  private:
    bool& m_running;
};

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
  keyboard_handler.register_dispatcher( local_event_dispatcher );

  while ( running )
  {
    the::time::Clock::Time now( clock.now() );

    keyboard_handler.check_keyboard( now );
    network_service.process_incoming_messages();

    world.broadcast( yarrr::TimerUpdate( now ) );
    world.in_focus();
    world.broadcast( yarrr::TimerUpdate( now ) );

    graphics_engine.update_screen();
    frequency_stabilizer.stabilize();
  }

  return 0;
}

