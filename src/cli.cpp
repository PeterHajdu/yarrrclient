#include "cli.hpp"
#include "text_token.hpp"
#include <yarrr/chat_message.hpp>
#include <yarrr/command.hpp>
#include <yarrr/log.hpp>
#include <theconf/configuration.hpp>
#include <regex>

const std::string yarrrc::Cli::m_prompt{ "$ " };

namespace yarrrc
{

Cli::Cli( int x, int y, yarrr::GraphicalEngine& graphical_engine )
  : m_text_box(
      { yarrrc::TextToken( m_prompt ) },
      graphical_engine,
      { x, y },
      { int( graphical_engine.screen_resolution().x ), 50 } )
{
}

void
Cli::append( const std::string& text )
{
  m_text += text;
  update_text_box();
}

void
Cli::update_text_box()
{
  m_text_box.set_content( { m_prompt, m_text } );
}

void
Cli::backspace()
{
  if ( m_text.empty() )
  {
    return;
  }

  m_text.pop_back();
  update_text_box();
}


void
Cli::finalize()
{
  if ( m_text.empty() )
  {
    return;
  }

  const char command_prefix{ '/' };
  if ( command_prefix == m_text.front() )
  {
    std::regex whitespaces{ "\\s+" };
    const int skip_slash{ 1 };
    std::sregex_token_iterator first{ m_text.begin() + skip_slash, m_text.end(), whitespaces, -1 };
    std::sregex_token_iterator last;
    dispatch( yarrr::Command( { first, last } ) );
    thelog( yarrr::log::debug )( "Command sent." );
  }
  else
  {
    dispatch( yarrr::ChatMessage( m_text, the::conf::get_value( "login_name" ) ) );
    thelog( yarrr::log::debug )( "Chat message sent." );
  }

  m_text.clear();
  update_text_box();
}

}

