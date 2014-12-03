#include "colorizer.hpp"
#include <thectci/hash.hpp>

namespace
{

constexpr float green_brightness{ 0.587 };
constexpr float blue_brightness{ 0.114 };
constexpr float red_brightness{ 0.299 };

constexpr int brightness_of( const yarrr::Colour& colour )
{
  return
    colour.red * red_brightness +
    colour.green * green_brightness +
    colour.blue * blue_brightness;
}

void increment_component( uint8_t& component )
{
  const float factor{ 1.2 };
  component = std::max( component, static_cast< uint8_t >( factor * component ) );
}

void brighten_colour( yarrr::Colour& colour )
{
  increment_component( colour.red );
  increment_component( colour.green );
  increment_component( colour.blue );
}

void normalize_colour( yarrr::Colour& colour, int brightness )
{
  while ( brightness_of( colour ) < brightness )
  {
    brighten_colour( colour );
  }
}

}

namespace yarrrc
{

//todo: colours should be cached in a fast lookup table
yarrr::Colour
colorize( const std::string& text, int brightness )
{
  uint32_t hash( the::ctci::hash( text.c_str() ) );
  yarrr::Colour colour{
    static_cast<uint8_t>( hash ),
    static_cast<uint8_t>( hash >> 8 ),
    static_cast<uint8_t>( hash >> 16 ),
    255u };

  normalize_colour( colour, brightness );
  return colour;
}

}


