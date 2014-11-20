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

class MissionControl
{
  public:
    MissionControl( yarrr::GraphicalEngine&, the::ctci::Dispatcher& mission_source );

  private:
    ListWindow::Lines generate_lines() const;
    yarrr::MissionContainer m_missions;
    yarrrc::ListWindow m_window;
    ListWindow::Lines m_mission_log;
};

}

