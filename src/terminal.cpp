#include "terminal.hpp"
#include "colorizer.hpp"
#include "window.hpp"
#include "text_token.hpp"
#include <yarrr/chat_message.hpp>
#include <yarrr/log.hpp>
#include <yarrr/command.hpp>
#include <yarrr/protocol.hpp>
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

  register_listener< yarrr::Command >(
      [ this ]( const yarrr::Command& command )
      {
        if ( command.command() != yarrr::Protocol::error )
        {
          return;
        }
        handle_error_message( command );
      } );
}

void
Terminal::add_line( TextToken::Container line )
{
  m_window.add_child( std::make_unique< yarrrc::TextBox >(
        line,
        m_graphical_engine,
        the::ui::Window::Coordinate{ 0, 0 },
        the::ui::Size{ chatwindow_width( m_graphical_engine ), 0 } ) );
}

void
Terminal::handle_error_message( const yarrr::Command& command )
{
  thelog( yarrr::log::error )( command.parameters().back() );
  add_line( tokenize(
      command.parameters().back(),
      yarrr::Colour::Red ) );
}

void
Terminal::handle_chat_message( const yarrr::ChatMessage& message )
{
  const int brightness( 100 );

  TextToken::Container chat_message( tokenize( message.message(), yarrr::Colour::White  ) );
  chat_message.insert( std::begin( chat_message ), yarrrc::TextToken{
      message.sender() + ": ",
      yarrrc::colorize( message.sender(), brightness ) } );
  add_line( std::move( chat_message ) );
}

}

