#pragma once

#include "window.hpp"

namespace yarrr
{

class PhysicalParameters;
class Object;
class Inventory;
class CargoSpace;
class TextToken;

}

namespace yarrrc
{

class Hud
{
  public:
    Hud( yarrr::GraphicalEngine&, const yarrr::Object& object );

  private:

    void print_character();
    void add_line( const TextToken& line );
    void update_window();

    yarrr::GraphicalEngine& m_graphical_engine;
    yarrr::PhysicalParameters& m_physical_parameters;
    yarrr::Inventory& m_inventory;
    yarrr::CargoSpace& m_cargo;
    yarrrc::Window m_window;
    the::ctci::SmartListener m_callback_token;
};

}

