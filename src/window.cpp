#include "window.hpp"
#include "text_token.hpp"

#include <yarrr/log.hpp>

namespace yarrrc
{

Window::Window(
    yarrr::GraphicalEngine& graphical_engine,
    const the::ui::Window::Coordinate& coordinate,
    const the::ui::Size& size,
    the::ui::Window::Restructure window_restructure )
  : the::ui::Window( coordinate, size, window_restructure )
  , yarrr::GraphicalObject( graphical_engine )
{
}

void
Window::draw() const
{
}


TextBox::TextBox(
    const TextToken::Container& content,
    yarrr::GraphicalEngine& graphical_engine,
    const the::ui::Window::Coordinate& coordinate,
    const the::ui::Size& size )
  : the::ui::TextBox< TextToken >( content, coordinate, size )
  , yarrr::GraphicalObject( graphical_engine )
{
  thelog( yarrr::log::insane )( "Text box created, size:", this->size().width, this->size().height );
}

void
TextBox::draw() const
{

  if ( !is_visible() )
  {
    thelog( yarrr::log::insane )( "Text box is invisible." );
    return;
  }

  thelog( yarrr::log::insane )( "Dawing text box." );
  const auto& top_left_corner( top_left() );

  int font_height( 0 );
  int x( top_left_corner.x );
  int y( top_left_corner.y );

  for ( const auto& line : lines() )
  {
    for ( const auto& token : line )
    {
      m_graphical_engine.print_text(
          x, y,
          token.text(),
          token.colour() );

      const auto size( m_graphical_engine.size_of_text( token.text() ) );
      x += size.width;
      font_height = size.height;
    }
    y += font_height;
    x = top_left_corner.x;
  }
}

}

