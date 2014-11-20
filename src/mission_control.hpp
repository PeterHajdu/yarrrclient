#pragma once

#include "list_window.hpp"
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
    ListWindow::Lines generate_lines() const;
    yarrr::MissionContainer m_missions;
    yarrrc::ListWindow m_window;
    ListWindow::Lines m_mission_log;
};

}

