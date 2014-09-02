#include "terminal.hpp"
#include <yarrr/chat_message.hpp>
#include <algorithm>
#include <thectci/hash.hpp>

namespace
{
  yarrr::Colour colorize( const std::string& text )
  {
    uint32_t hash( the::ctci::hash( text.c_str() ) );
    return {
      static_cast<uint8_t>( hash ),
      static_cast<uint8_t>( hash >> 8 ),
      static_cast<uint8_t>( hash >> 16 ),
      255u };
  }
}

namespace yarrrc
{

Terminal::Terminal(
    yarrr::GraphicalEngine& engine,
    int number_of_messages )
  : yarrr::GraphicalObject( engine )
  , m_number_of_shown_messages( number_of_messages )
  , m_lines_to_scroll( number_of_messages / 2 )
  , m_first_message_index( 0 )
{
  register_listener< yarrr::ChatMessage >(
      std::bind( &Terminal::handle_chat_message, this, std::placeholders::_1 ) );
}

void
Terminal::jump_to_last_page()
{
  m_first_message_index = int( m_messages.size() ) - m_number_of_shown_messages;
  normalize_first_index();
}

void
Terminal::handle_chat_message( const yarrr::ChatMessage& message )
{
  m_messages.push_back( {
        { message.sender() + ": ", colorize( message.sender() ) },
        { message.message(), { 255, 255, 255, 255 } } } );
  jump_to_last_page();
}

void
Terminal::draw() const
{
  const int length( std::min( int( m_messages.size() ), m_number_of_shown_messages ) );

  for ( int i( 0 ); i < length; ++i )
  {
    m_graphical_engine.print_text_tokens(
        0, i * yarrr::GraphicalEngine::font_height,
        m_messages[ m_first_message_index + i ] );
  }
}

void
Terminal::home()
{
  m_first_message_index = 0;
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
}

void
Terminal::scroll_down()
{
  m_first_message_index += m_lines_to_scroll;
  normalize_first_index();
}

void
Terminal::normalize_first_index()
{
  m_first_message_index = std::min( m_first_message_index, int( m_messages.size() ) - m_number_of_shown_messages );
  m_first_message_index = std::max( 0, m_first_message_index );
}

}

