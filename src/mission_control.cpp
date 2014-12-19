#include "mission_control.hpp"
#include "local_event_dispatcher.hpp"
#include "wakeup.hpp"
#include <thectci/dispatcher.hpp>
#include <yarrr/mission.hpp>
#include <yarrr/log.hpp>
#include <theui/list_restructure.hpp>
#include <unordered_map>
namespace
{

std::unordered_map< int, yarrr::Colour > state_to_colour = {
  { yarrr::ongoing, yarrr::Colour::White },
  { yarrr::succeeded, yarrr::Colour::Green },
  { yarrr::failed, yarrr::Colour::Red } };

const yarrr::Colour blue{ 150, 150, 255, 255 };
const the::ui::Size mission_control_size{ 500, 200 };

the::ui::Window::Coordinate
mission_window_top_left_from( yarrr::GraphicalEngine& graphical_engine )
{
  return { 0, int( graphical_engine.screen_resolution().y - mission_control_size.height ) };
}

class MissionWindow : public yarrrc::Window
{
  public:
    MissionWindow(
        yarrr::GraphicalEngine& graphical_engine,
        const yarrr::Mission& mission )
      : yarrrc::Window(
          graphical_engine,
          mission_window_top_left_from( graphical_engine ),
          mission_control_size,
          the::ui::front_from_top_with_fixed_height )
    {
      thelog( yarrr::log::debug )(
          "Creating mission window for:", mission.name(),
          "size:", size().width, size().height,
          "coordinate:", top_left().x, top_left().y );
      add_title_from( mission );
      add_objectives_from( mission );
    }

  private:
    void add_title_from( const yarrr::Mission& mission )
    {
      const auto title_content( yarrrc::tokenize( mission.name() + " -> " + mission.description(), blue ) );

      title_window = new yarrrc::TextBox(
            title_content,
            m_graphical_engine,
            the::ui::Window::Coordinate{ 0, 0 },
            the::ui::Size{ 0, 0 } );

      add_child( the::ui::Window::Pointer( title_window ) );
    }

    void add_objectives_from( const yarrr::Mission& mission )
    {
      auto& objectives_window( add_child( std::make_unique< the::ui::Window >(
            the::ui::Window::Coordinate{ 0, 0 },
            the::ui::Size{ 0, size().height - title_window->size().height },
            the::ui::back_from_bottom_with_fixed_height ) ) );

      for ( const auto& objective : mission.objectives() )
      {
        objectives_window.add_child( std::make_unique< yarrrc::TextBox >(
              yarrrc::tokenize( objective.description(), state_to_colour[ objective.state() ] ),
              m_graphical_engine,
              the::ui::Window::Coordinate{ 0, 0 },
              the::ui::Size{ 0, 0 } ) );
      }
    }

    yarrrc::TextBox* title_window;
};

}

namespace yarrrc
{

MissionControl::MissionControl( yarrr::GraphicalEngine& graphical_engine, the::ctci::Dispatcher& mission_source )
  : m_missions( [ this ]( const yarrr::Mission& mission )
      { handle_mission_finished( mission ); } )
  , m_graphical_engine( graphical_engine )
{
  mission_source.register_listener< yarrr::Mission >(
      [ this ]( const yarrr::Mission& mission )
      {
        thelog( yarrr::log::debug )( "Mission update arrived:", mission.id(), mission.name(), mission.state() );
        m_missions.add_mission( yarrr::Mission::Pointer( new yarrr::Mission( mission ) ) );
      } );

  call_every_second(
      [ this ]( const EverySecond& )
      {
        update_window();
      } );
}

void
MissionControl::handle_mission_finished( const yarrr::Mission& mission )
{
  thelog( yarrr::log::debug )( "Mission finished.", mission.id() );
  yarrrc::local_dispatch( yarrrc::MissionFinished( mission ) );
}

void
MissionControl::update_window()
{
  if ( m_missions.missions().empty() )
  {
    m_window.reset();
    return;
  }

  m_window = std::make_unique< MissionWindow >(
      m_graphical_engine,
      *m_missions.missions()[ 0 ] );
  thelog( yarrr::log::insane )( "Mission window dimensions:",
      "size: ", m_window->size().width, m_window->size().height,
      "coordinate: ", m_window->top_left().x, m_window->top_left().y );
}

void
MissionControl::update()
{
  thelog( yarrr::log::insane )( "MissionControl update called." );
  m_missions.update();
}

}

