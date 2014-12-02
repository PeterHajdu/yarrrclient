#include "colorizer.hpp"
#include <thectci/hash.hpp>

namespace
{

constexpr int lightness_of( const yarrr::Colour& colour )
{
  return colour.red + colour.green + colour.blue;
}

void lighten_colour( yarrr::Colour& colour )
{
  const float factor{ 1.1 };
  colour.red *= factor;
  colour.green *= factor;
  colour.blue *= factor;
}

void normalize_colour( yarrr::Colour& colour )
{
  while ( lightness_of( colour ) < 300 )
  {
    lighten_colour( colour );
  }
}

}

namespace yarrrc
{

//todo: colours should be cached in a fast lookup table
yarrr::Colour
colorize( const std::string& text )
{
  uint32_t hash( the::ctci::hash( text.c_str() ) );
  yarrr::Colour colour{
    static_cast<uint8_t>( hash ),
    static_cast<uint8_t>( hash >> 8 ),
    static_cast<uint8_t>( hash >> 16 ),
    255u };

  normalize_colour( colour );
  return colour;
}

}


