#pragma once

#include "window.hpp"
#include <thetime/clock.hpp>

namespace yarrrc
{

class InformationWindow
{
  public:
    InformationWindow( yarrr::GraphicalEngine& );

  private:
    void update_time_to( const the::time::Time& );
    TextBox m_window;
};

}

