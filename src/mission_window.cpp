#include "mission_window.hpp"
#include <thectci/dispatcher.hpp>

namespace yarrrc
{

MissionWindow::MissionWindow( yarrr::GraphicalEngine& engine, the::ctci::Dispatcher& mission_source )
  : m_missions( []( const yarrr::Mission& ){} )
  , m_window( 0, 120, engine, [ this ](){ return generate_lines(); } )
{
  mission_source.register_listener< yarrr::Mission >(
      [ this ]( const yarrr::Mission& mission )
      {
        m_missions.add_mission( yarrr::Mission::Pointer( new yarrr::Mission( mission ) ) );
      } );
}

ListWindow::Lines
MissionWindow::generate_lines() const
{
  ListWindow::Lines lines;
  for ( const auto& mission : m_missions.missions() )
  {
    lines.push_back( { mission->name(), yarrr::Colour::White } );
    lines.push_back( { mission->description(), yarrr::Colour::White } );

    for ( const auto& objective : mission->objectives() )
    {
      lines.push_back( { objective.description(), yarrr::Colour::White } );
    }
  }
  return lines;
}

}

