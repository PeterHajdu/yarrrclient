#pragma once

#include <yarrr/graphical_engine.hpp>

namespace yarrrc
{

class ListWindow : yarrr::GraphicalObject
{
  public:
    using Lines = std::vector< std::string >;
    using LineGenerator = std::function< Lines() >;
    ListWindow( int x, int y, yarrr::GraphicalEngine&, LineGenerator generator );

  private:
    virtual void draw() const override;

    LineGenerator generate_lines;
    const int m_x;
    const int m_y;
};

}

