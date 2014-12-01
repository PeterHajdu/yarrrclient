#include "mission_control.hpp"
#include "local_event_dispatcher.hpp"
#include <thectci/dispatcher.hpp>
#include <yarrr/mission.hpp>
#include <thectci/service_registry.hpp>
#include <unordered_map>
namespace
{

std::unordered_map< int, yarrr::Colour > state_to_colour = {
  { yarrr::ongoing, yarrr::Colour::White },
  { yarrr::succeeded, yarrr::Colour::Green },
  { yarrr::failed, yarrr::Colour::Red } };

const yarrr::Colour blue{ 100, 100, 255, 255 };

}

namespace yarrrc
{

MissionControl::MissionControl( yarrr::GraphicalEngine& engine, the::ctci::Dispatcher& mission_source )
  : m_missions( [ this ]( const yarrr::Mission& mission )
      { handle_mission_finished( mission ); } )
  , m_window( 0, 150, engine, [ this ](){ return generate_lines(); } )
{
  mission_source.register_listener< yarrr::Mission >(
      [ this ]( const yarrr::Mission& mission )
      {
        thelog( yarrr::log::debug )( "Mission update arrived:", mission.id(), mission.name(), mission.state() );
        m_missions.add_mission( yarrr::Mission::Pointer( new yarrr::Mission( mission ) ) );
      } );
}

void
MissionControl::handle_mission_finished( const yarrr::Mission& mission )
{
  thelog( yarrr::log::debug )( "Mission finished.", mission.id() );
  m_mission_log.push_back( { mission.name(), state_to_colour[ mission.state() ] } );
  yarrrc::local_dispatch( yarrrc::MissionFinished( mission ) );
}

void
MissionControl::append_finished_missions( ListWindow::Lines& lines ) const
{
  if ( m_mission_log.empty() )
  {
    return;
  }

  lines.push_back( { "Finished missions:", blue } );
  lines.insert( std::end( lines ), std::begin( m_mission_log ), std::end( m_mission_log ) );
}

void
MissionControl::append_ongoing_missions( ListWindow::Lines& lines ) const
{
  if ( m_missions.missions().empty() )
  {
    return;
  }

  lines.push_back( { "Current missions:", blue } );
  for ( const auto& mission : m_missions.missions() )
  {
    lines.push_back( { mission->name() + " | " + mission->description(), blue } );

    for ( const auto& objective : mission->objectives() )
    {
      lines.push_back( { objective.description(), state_to_colour[ objective.state() ] } );
    }
  }
}

ListWindow::Lines
MissionControl::generate_lines() const
{
  ListWindow::Lines lines;
  append_finished_missions( lines );
  append_ongoing_missions( lines );
  return lines;
}

void
MissionControl::update()
{
  m_missions.update();
}

}

