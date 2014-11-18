#pragma once

#include "list_window.hpp"

namespace yarrr
{

class PhysicalParameters;
class Object;
class Inventory;

}

namespace yarrrc
{

class Hud
{
  public:
    Hud( yarrr::GraphicalEngine&, const yarrr::Object& object );

  private:
    ListWindow::Lines build_hud_lines() const;

    yarrr::PhysicalParameters& m_physical_parameters;
    yarrr::Inventory& m_inventory;
    yarrrc::ListWindow m_window;
};

}

