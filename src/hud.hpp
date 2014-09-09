#pragma once

#include <yarrr/graphical_engine.hpp>

namespace yarrr
{

class PhysicalParameters;
class Object;
class Inventory;

}

namespace yarrrc
{

class Hud : public yarrr::GraphicalObject
{
  public:
    Hud( yarrr::GraphicalEngine&, const yarrr::Object& object );
    virtual void draw() const override;
    virtual ~Hud() = default;
  private:
    typedef std::vector< std::string > Lines;
    Lines build_hud_lines() const;
    void print_lines( const Lines& ) const;

    yarrr::PhysicalParameters& m_physical_parameters;
    yarrr::Inventory& m_inventory;
    const int m_height_of_screen;
};

}

