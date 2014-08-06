#pragma once

#include <yarrr/object.hpp>
#include <yarrr/physical_parameters.hpp>

namespace yarrr
{
  class DeleteObject;
  class ObjectStateUpdate;
  class Command;
  class ObjectContainer;
}

class ConnectionEstablished;
class LoggedIn;

class World
{
  public:
    World( yarrr::ObjectContainer& );
    void in_focus();

  private:
    void handle_command( const yarrr::Command& command );
    void handle_connection_established( const ConnectionEstablished& connection_established );
    void handle_login( const LoggedIn& login );
    void handle_delete_object( const yarrr::DeleteObject& delete_object );

    yarrr::ObjectContainer& m_objects;
    the::ctci::Dispatcher m_dispatcher;
    yarrr::Object* m_my_ship;
};

