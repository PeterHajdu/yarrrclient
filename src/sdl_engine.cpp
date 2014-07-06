#include "sdl_engine.hpp"

#include <yarrr/object.hpp>

#include <cmath>
#include <cassert>
#include <SDL2/SDL.h>

SdlEngine::SdlEngine( int32_t x, int32_t y )
  : m_window( nullptr )
  , m_screen( nullptr )
  , m_x( x )
  , m_y( y )
  , m_center{ 400, 400 }
{
  assert(
      SDL_Init( SDL_INIT_VIDEO ) == 0 &&
      "Unable to initialize sdl!" );
  m_window = SDL_CreateWindow(
      "yarrr",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      x, y,
      SDL_WINDOW_SHOWN );
  assert( m_window );
  m_screen = SDL_GetWindowSurface( m_window );
  assert( m_screen );
}


SdlEngine::~SdlEngine()
{
  SDL_DestroyWindow( m_window );
  SDL_Quit();
}


void
SdlEngine::update_screen()
{
  draw_background();
  draw_objects();
  SDL_UpdateWindowSurface( m_window );
}


void
SdlEngine::draw_point(
    int32_t x,
    int32_t y,
    int size, uint32_t colour )
{
  const int32_t new_x( x - m_center.x - size / 2 + m_x / 2 );
  const int32_t new_y( y - m_center.y - size / 2 + m_y / 2 );

  SDL_Rect rectangle = {
    static_cast<Sint16>( new_x ),
    static_cast<Sint16>( new_y ),
    static_cast<Uint16>( size ),
    static_cast<Uint16>( size ) };

  SDL_FillRect( m_screen, &rectangle, colour );
}


void
SdlEngine::focus_to( const yarrr::Coordinate& center )
{
  m_center = center;
}


void
SdlEngine::draw_ship( const yarrr::Object& ship )
{
  int32_t x( ship.coordinate.x );
  int32_t y( ship.coordinate.y );

  int32_t head_x( cos( ship.angle * 3.14 / 180.0 / 4.0 ) * 20.0 );
  int32_t head_y( sin( ship.angle * 3.14 / 180.0 / 4.0 ) * 20.0 );

  draw_point( x, y, 4, 0xaaaa00 );
  draw_point( x + head_x, y + head_y, 4, 0x00ff00 );
  draw_point( x - head_x, y - head_y, 4, 0xff0000 );

  int32_t perp_x( head_y );
  int32_t perp_y( -1 * head_x );

  draw_point( x - head_x * 0.5 + perp_x * 0.5, y - head_y * 0.5 + perp_y * 0.5, 4, 0xff0000 );
  draw_point( x - head_x * 0.5 - perp_x * 0.5, y - head_y * 0.5 - perp_y * 0.5, 4, 0xff0000 );
}


void
SdlEngine::draw_objects()
{
  for ( auto& object : m_objects )
  {
    object->draw();
  }
}


void
SdlEngine::draw_background()
{
  SDL_Rect rectangle =
  { static_cast<unsigned short>( 0 ), static_cast<unsigned short>( 0 ),
    static_cast<unsigned short>( m_x ), static_cast<unsigned short>( m_y ) };
  SDL_FillRect( m_screen, &rectangle, 0x000000 );
}


void
SdlEngine::register_object( DrawableObject& object )
{
  m_objects.push_back( &object );
}

