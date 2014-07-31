#pragma once

#include <yarrr/object.hpp>
#include <yarrr/physical_parameters.hpp>
#include <yarrr/object_container.hpp>

namespace yarrr
{
  class DeleteObject;
  class ObjectStateUpdate;
  class Command;
}

class ConnectionEstablished;
class LoggedIn;

class World
{
  public:
    World();
    void handle_connection_established( const ConnectionEstablished& connection_established );
    void handle_login( const LoggedIn& login );
    void in_focus();
    void handle_command( const yarrr::Command& command );

    template < typename Event >
    void broadcast( const Event& event )
    {
      m_objects.dispatch( event );
    }

    void handle_delete_object( const yarrr::DeleteObject& delete_object );
    void handle_object_state_update( const yarrr::ObjectStateUpdate& object_state_update );

  private:
    yarrr::ObjectContainer m_objects;

    the::ctci::Dispatcher m_dispatcher;
    yarrr::Object* m_my_ship;
};

