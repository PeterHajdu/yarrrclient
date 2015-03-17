#include "terminal.hpp"
#include "colorizer.hpp"
#include "window.hpp"
#include "text_token.hpp"
#include <yarrr/chat_message.hpp>
#include <algorithm>
#include <theui/list_restructure.hpp>

namespace
{

int
chatwindow_width( const yarrr::GraphicalEngine& graphical_engine )
{
  return static_cast< int >( graphical_engine.screen_resolution().x - 1 );
}

}

namespace yarrrc
{

Terminal::Terminal( yarrr::GraphicalEngine& graphical_engine )
  : m_window(
      graphical_engine,
      { 0, 0 },
      { chatwindow_width( graphical_engine ), 100 },
      the::ui::back_from_bottom_with_fixed_height )
  , m_graphical_engine( graphical_engine )
{
  register_listener< yarrr::ChatMessage >(
      std::bind( &Terminal::handle_chat_message, this, std::placeholders::_1 ) );
}

void
Terminal::handle_chat_message( const yarrr::ChatMessage& message )
{
  const int brightness( 100 );
  const TextToken::Container tokenized_message{
    yarrrc::TextToken{
      message.sender() + ": ",
      yarrrc::colorize( message.sender(), brightness ) },
    yarrrc::TextToken{
      message.message(),
      yarrr::Colour::White } };

  m_window.add_child( std::make_unique< yarrrc::TextBox >(
        tokenized_message,
        m_graphical_engine,
        the::ui::Window::Coordinate{ 0, 0 },
        the::ui::Size{ chatwindow_width( m_graphical_engine ), 0 } ) );
}

}

