#include "fps.hpp"
#include <thetime/clock.hpp>
#include <thectci/service_registry.hpp>

FpsDrawer::FpsDrawer( the::time::Clock& clock )
  : GraphicalObject( the::ctci::service< yarrr::GraphicalEngine >() )
  , m_frequency_meter( clock )
{
  m_frequency_meter.tick();
}

void
FpsDrawer::draw() const
{
  m_frequency_meter.tick();
  m_graphical_engine.print_text( 0, 300, "fps: ", yarrr::Colour::White );
  m_graphical_engine.print_text( 50, 300, std::to_string( m_frequency_meter.per_second() ), yarrr::Colour::White );
}

