#pragma once

#include <yarrr/object.hpp>
#include <yarrr/physical_parameters.hpp>
#include "hud.hpp"

namespace yarrr
{
  class DeleteObject;
  class ObjectStateUpdate;
  class ShipControl;
  class ObjectContainer;
  class ObjectAssigned;
}

class ConnectionEstablished;

namespace yarrrc
{

class World
{
  public:
    World( yarrr::ObjectContainer& );
    void in_focus();

  private:
    void handle_object_update( const yarrr::ObjectUpdate& );
    void handle_object_init( const yarrr::ObjectUpdate& );
    void handle_command( const yarrr::ShipControl& command );
    void handle_connection_established( const ConnectionEstablished& connection_established );
    void handle_login( const yarrr::ObjectAssigned& login );
    void handle_delete_object( const yarrr::DeleteObject& delete_object );

    yarrr::ObjectContainer& m_objects;
    yarrr::Object::Id m_my_ship_id;
    yarrr::Object* m_my_ship;

    std::unique_ptr< yarrrc::Hud > m_hud;
};

}

