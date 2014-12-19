#pragma once

#include <yarrr/graphical_engine.hpp>
#include <vector>

namespace yarrrc
{

class TextToken
{
  public:
    using Container = std::vector< TextToken >;
    TextToken() = default;

    TextToken(
        std::string text,
        yarrr::Colour colour = yarrr::Colour::White );

    //todo: height and width should be calculated by sdl engine
    int width() const;
    int height() const;
    const std::string& text() const;
    const yarrr::Colour& colour() const;

  private:
    std::string m_text;
    yarrr::Colour m_colour;
};

std::ostream& operator<<( std::ostream&, const TextToken& );

//only texts are compared, colour is ignored
bool
operator==( const TextToken&, const TextToken& );

TextToken::Container
tokenize( const std::string& text, const yarrr::Colour& );

}

