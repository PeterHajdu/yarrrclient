#pragma once

#include "window.hpp"
#include "text_token.hpp"
#include <yarrr/mission_container.hpp>

namespace the
{
namespace ctci
{
class Dispatcher;
}
}

namespace yarrrc
{

class MissionFinished
{
  public:
    add_ctci( "yarrrc_mission_finished" );
    MissionFinished( const yarrr::Mission& mission )
      : mission( mission )
    {
    }

    const yarrr::Mission& mission;
};

class MissionControl
{
  public:
    MissionControl( yarrr::GraphicalEngine&, the::ctci::Dispatcher& mission_source );
    void update();

  private:
    void handle_mission_finished( const yarrr::Mission& );
    void update_window();

    yarrr::MissionContainer m_missions;
    yarrr::GraphicalEngine& m_graphical_engine;
    the::ui::Window::Pointer m_window;
};

}

