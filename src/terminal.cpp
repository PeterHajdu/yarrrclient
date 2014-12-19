#include "terminal.hpp"
#include "colorizer.hpp"
#include "text_token.hpp"
#include <yarrr/chat_message.hpp>
#include <algorithm>
#include <theui/list_restructure.hpp>

namespace yarrrc
{

Terminal::Terminal(
    yarrr::GraphicalEngine& graphical_engine,
    int number_of_messages )
  : m_number_of_shown_messages( number_of_messages )
  , m_lines_to_scroll( number_of_messages / 2 )
  , m_first_message_index( 0 )
  , m_window(
      graphical_engine,
      { 0, 0 },
      {
        static_cast< int >( graphical_engine.screen_resolution().x - 1 ),
        100 },
      the::ui::front_from_top_with_fixed_height )
  , m_graphical_engine( graphical_engine )
{
  register_listener< yarrr::ChatMessage >(
      std::bind( &Terminal::handle_chat_message, this, std::placeholders::_1 ) );
}

void
Terminal::jump_to_last_page()
{
  m_first_message_index = int( m_messages.size() ) - m_number_of_shown_messages;
  normalize_first_index();
  update_window();
}

void
Terminal::handle_chat_message( const yarrr::ChatMessage& message )
{
  m_messages.push_back( TextToken::Container{
      { message.sender() + ": ", yarrrc::colorize( message.sender(), 100 ) },
      { message.message(), { 255, 255, 255, 255 } } } );
  jump_to_last_page();
}

void
Terminal::update_window()
{
  const int length( std::min( int( m_messages.size() ), m_number_of_shown_messages ) );

  m_window.clear();
  for ( int i( 0 ); i < length; ++i )
  {
    const auto& message( m_messages[ m_first_message_index + i ] );
    m_window.add_child( std::make_unique< TextBox >(
          message,
          m_graphical_engine,
          the::ui::Window::Coordinate{ 0, 0 },
          the::ui::Size{ 10000, message.back().height() } ) );
  }
}

void
Terminal::home()
{
  m_first_message_index = 0;
  update_window();
}

void
Terminal::end()
{
  jump_to_last_page();
}

void
Terminal::scroll_up()
{
  m_first_message_index -= m_lines_to_scroll;
  normalize_first_index();
  update_window();
}

void
Terminal::scroll_down()
{
  m_first_message_index += m_lines_to_scroll;
  normalize_first_index();
  update_window();
}

void
Terminal::normalize_first_index()
{
  m_first_message_index = std::min( m_first_message_index, int( m_messages.size() ) - m_number_of_shown_messages );
  m_first_message_index = std::max( 0, m_first_message_index );
}

}

