#include "hud.hpp"
#include <yarrr/physical_parameters.hpp>
#include <yarrr/inventory.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/object.hpp>

namespace yarrrc
{


Hud::Hud( yarrr::GraphicalEngine& graphical_engine, const yarrr::Object& object )
  : GraphicalObject( graphical_engine )
  , m_physical_parameters( yarrr::component_of< yarrr::PhysicalBehavior >( object ).physical_parameters )
  , m_inventory( yarrr::component_of< yarrr::Inventory >( object ) )
  , m_height_of_screen( m_graphical_engine.screen_resolution().y )
{
  (void)m_physical_parameters;
}

void
Hud::draw() const
{
  print_lines( build_hud_lines() );
}


Hud::Lines
Hud::build_hud_lines() const
{
  Lines lines;
  lines.push_back( "inventory: " );

  for ( const auto& item : m_inventory.items() )
  {
    lines.push_back( " -> " + item.get().name() );
  }

  lines.push_back( "integrity: " +
      std::to_string( m_physical_parameters.integrity ) );

  lines.push_back( "coordinate: " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.coordinate.x ) ) + " , " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.coordinate.y ) ) );

  lines.push_back( "velocity: " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.velocity.x ) ) + " , " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.velocity.y ) ) );

  lines.push_back( "orientation: " +
      std::to_string( yarrr::hiplon_to_degrees( m_physical_parameters.orientation ) ) );

  lines.push_back( "angular velocity: " +
      std::to_string( yarrr::hiplon_to_degrees( m_physical_parameters.angular_velocity ) ) );

  return lines;
}


void
Hud::print_lines( const Lines& lines ) const
{
  size_t y_coordinate_of_line{ m_height_of_screen - lines.size() * yarrr::GraphicalEngine::font_height };
  for ( const auto& line: lines )
  {
    m_graphical_engine.print_text( 0, y_coordinate_of_line, line, yarrr::Colour::White );
    y_coordinate_of_line += yarrr::GraphicalEngine::font_height;
  }
}

}

