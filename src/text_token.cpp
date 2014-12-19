#include "text_token.hpp"
#include <yarrr/graphical_engine.hpp>
#include <thectci/service_registry.hpp>

namespace yarrrc
{

TextToken::TextToken(
    std::string text,
    yarrr::Colour colour )
  : m_text{ std::move( text ) }
  , m_colour( colour )
{
}

int
TextToken::width() const
{
  auto& graphical_engine( the::ctci::service< yarrr::GraphicalEngine >() );
  return graphical_engine.size_of_text( m_text ).width;
}

int
TextToken::height() const
{
  auto& graphical_engine( the::ctci::service< yarrr::GraphicalEngine >() );
  return graphical_engine.size_of_text( m_text ).height;
}

const std::string&
TextToken::text() const
{
  return m_text;
}

const yarrr::Colour&
TextToken::colour() const
{
  return m_colour;
}

TextToken::Container
tokenize( const std::string& text, const yarrr::Colour& colour )
{
  TextToken::Container tokens;
  const char* delimiters{ " \t\n" };
  std::size_t start_index{ 0 };

  while ( true )
  {
    const auto index_of_delimiter( text.find_first_of(
        delimiters,
        start_index ) );

    if ( std::string::npos == index_of_delimiter )
    {
      break;
    }

    const auto length_of_word( index_of_delimiter - start_index );
    tokens.emplace_back(
        text.substr( start_index, length_of_word ) + " ",
        colour );
    start_index = index_of_delimiter + 1;
  }

  if ( start_index < text.length() )
  {
    tokens.emplace_back(
        text.substr( start_index ),
        colour );
  }

  return tokens;
}

bool
operator==( const TextToken& l, const TextToken& r )
{
  return l.text() == r.text();
}

std::ostream&
operator<<( std::ostream& output, const TextToken& token )
{
  output << token.text();
  return output;
}

}

