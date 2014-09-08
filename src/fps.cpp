#include "fps.hpp"
#include <thetime/clock.hpp>
#include <thectci/service_registry.hpp>

namespace
{

yarrr::Coordinate
calculate_position_from( const yarrr::Coordinate& screen_resolution )
{
  return { screen_resolution.x - 90, screen_resolution.y - 20 };
}

}

namespace yarrrc
{

FpsDrawer::FpsDrawer( the::time::Clock& clock )
  : GraphicalObject( the::ctci::service< yarrr::GraphicalEngine >() )
  , m_frequency_meter( clock )
  , m_position( calculate_position_from( m_graphical_engine.screen_resolution() ) )
{
  m_frequency_meter.tick();
}

void
FpsDrawer::draw() const
{
  m_frequency_meter.tick();
  m_graphical_engine.print_text( m_position.x, m_position.y, "fps: ", yarrr::Colour::White );
  m_graphical_engine.print_text( m_position.x + 50, m_position.y, std::to_string( m_frequency_meter.per_second() ), yarrr::Colour::White );
}

}

