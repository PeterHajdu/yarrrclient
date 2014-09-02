#include "cli.hpp"
#include <yarrr/chat_message.hpp>
#include <theconf/configuration.hpp>

const std::string yarrrc::Cli::m_prompt{ "$ " };

namespace yarrrc
{

Cli::Cli( int x, int y, yarrr::GraphicalEngine& graphical_engine )
  : GraphicalObject( graphical_engine )
  , m_x( x )
  , m_y( y )
{
}


void
Cli::draw() const
{
  m_graphical_engine.print_text(
      m_x, m_y,
      m_prompt + m_text,
      { 255, 255, 255, 255 } );
}


void
Cli::append( const std::string& text )
{
  m_text += text;
}


void
Cli::backspace()
{
  if ( m_text.empty() )
  {
    return;
  }

  m_text.pop_back();
}


void
Cli::finalize()
{
  if ( m_text.empty() )
  {
    return;
  }

  dispatch( yarrr::ChatMessage( m_text, the::conf::get_value( "login_name" ) ) );
  m_text.clear();
}

}

