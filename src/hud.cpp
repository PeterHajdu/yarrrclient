#include "hud.hpp"
#include <yarrr/physical_parameters.hpp>
#include <yarrr/inventory.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/object.hpp>

namespace
{
  int calculate_x_from( const yarrr::Coordinate& screen_resolution )
  {
    return screen_resolution.x - 250;
  }

}

namespace yarrrc
{

Hud::Hud( yarrr::GraphicalEngine& graphical_engine, const yarrr::Object& object )
  : m_physical_parameters( yarrr::component_of< yarrr::PhysicalBehavior >( object ).physical_parameters )
  , m_inventory( yarrr::component_of< yarrr::Inventory >( object ) )
  , m_window( calculate_x_from( graphical_engine.screen_resolution() ), 120, graphical_engine, [ this ](){ return build_hud_lines(); } )
{
}

ListWindow::Lines
Hud::build_hud_lines() const
{
  ListWindow::Lines lines;
  lines.push_back( { "inventory: ", yarrr::Colour::White } );

  for ( const auto& item : m_inventory.items() )
  {
    lines.push_back( { " -> " + item.get().name(), yarrr::Colour::White } );
  }

  lines.push_back( { "integrity: " +
      std::to_string( m_physical_parameters.integrity ), yarrr::Colour::White } );

  lines.push_back( { "coordinate: " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.coordinate.x ) ) + " , " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.coordinate.y ) ), yarrr::Colour::White } );

  lines.push_back( { "velocity: " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.velocity.x ) ) + " , " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.velocity.y ) ), yarrr::Colour::White } );

  lines.push_back( { "orientation: " +
      std::to_string( yarrr::hiplon_to_degrees( m_physical_parameters.orientation ) ), yarrr::Colour::White } );

  lines.push_back( { "angular velocity: " +
      std::to_string( yarrr::hiplon_to_degrees( m_physical_parameters.angular_velocity ) ), yarrr::Colour::White } );

  return lines;
}

}

