#include "colorizer.hpp"
#include <thectci/hash.hpp>

namespace yarrrc
{

yarrr::Colour
colorize( const std::string& text )
{
  uint32_t hash( the::ctci::hash( text.c_str() ) );
  return {
    static_cast<uint8_t>( hash ),
    static_cast<uint8_t>( hash >> 8 ),
    static_cast<uint8_t>( hash >> 16 ),
    255u };
}

}


