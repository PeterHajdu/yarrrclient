#pragma once

#include "list_window.hpp"
namespace the
{
namespace time
{
class Clock;
}
}

namespace yarrrc
{

class InformationWindow
{
  public:
    InformationWindow( yarrr::GraphicalEngine&, const the::time::Clock& );

  private:
    ListWindow::Lines generate_lines() const;
    ListWindow m_window;
    const the::time::Clock& m_clock;
};

}

