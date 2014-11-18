#include "list_window.hpp"
#include <iostream>

namespace yarrrc
{

ListWindow::ListWindow( int x, int y, yarrr::GraphicalEngine& graphical_engine, LineGenerator generator )
  : GraphicalObject( graphical_engine )
  , generate_lines( generator )
  , m_x( x )
  , m_y( y )
{
}


void
ListWindow::draw() const
{
  int y_coordinate_of_line{ m_y };
  for ( const auto& line : generate_lines() )
  {
    m_graphical_engine.print_text( m_x,y_coordinate_of_line, line, yarrr::Colour::White );
    y_coordinate_of_line += yarrr::GraphicalEngine::font_height;
  }
}

}

