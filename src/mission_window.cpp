#include "mission_window.hpp"
#include <thectci/dispatcher.hpp>

namespace yarrrc
{

MissionWindow::MissionWindow( yarrr::GraphicalEngine& engine, the::ctci::Dispatcher& mission_source )
  : GraphicalObject( engine )
  , m_missions( []( const yarrr::Mission& ){} )
{
  mission_source.register_listener< yarrr::Mission >(
      [ this ]( const yarrr::Mission& mission )
      {
        m_missions.add_mission( yarrr::Mission::Pointer( new yarrr::Mission( mission ) ) );
      } );
}

void
MissionWindow::draw() const
{
  for ( const auto& mission : m_missions.missions() )
  {
    m_graphical_engine.print_text( 0, 0, mission->name(), yarrr::Colour::White );
    m_graphical_engine.print_text( 0, 0, mission->description(), yarrr::Colour::White );

    for ( const auto& objective : mission->objectives() )
    {
      m_graphical_engine.print_text( 0, 0, objective.description(), yarrr::Colour::White );
    }
  }
}

}

