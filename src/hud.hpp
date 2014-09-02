#pragma once

#include <yarrr/graphical_engine.hpp>

namespace yarrrc
{
class PhysicalParameters;

class Hud : public yarrr::GraphicalObject
{
  public:
    Hud( yarrr::GraphicalEngine&, yarrr::PhysicalParameters& );
    virtual void draw() const override;
    virtual ~Hud() = default;
  private:
    typedef std::vector< std::string > Lines;
    Lines build_hud_lines() const;
    void print_lines( const Lines& ) const;
    yarrr::PhysicalParameters& m_physical_parameters;
};

}

