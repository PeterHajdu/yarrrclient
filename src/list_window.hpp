#pragma once

#include <yarrr/graphical_engine.hpp>

namespace yarrrc
{

class ListWindow : yarrr::GraphicalObject
{
  public:
    using Lines = std::vector< std::string >;
    using LinesRef = const Lines&;
    using LineGenerator = std::function< LinesRef() >;
    ListWindow( int x, int y, yarrr::GraphicalEngine&, LineGenerator generator );

  private:
    virtual void draw() const override;

    LineGenerator generate_lines;
    const int m_x;
    const int m_y;
};

}

