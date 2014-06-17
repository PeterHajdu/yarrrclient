#include "sdl_engine.hpp"

#include <yarrr/ship.hpp>

#include <cassert>
#include <SDL2/SDL.h>

SdlEngine::SdlEngine( uint32_t x, uint32_t y )
  : m_window( nullptr )
  , m_screen( nullptr )
  , m_x( x )
  , m_y( y )
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
  //draw_background();
  draw_background();
  draw_objects();
  SDL_UpdateWindowSurface( m_window );
}


void
SdlEngine::draw_rectangle(
    uint32_t x,
    uint32_t y,
    int size, uint32_t colour )
{
  SDL_Rect rectangle = {
    static_cast<Sint16>( x - size / 2 ),
    static_cast<Sint16>( y - size / 2 ),
    static_cast<Uint16>( size ),
    static_cast<Uint16>( size ) };

  SDL_FillRect( m_screen, &rectangle, colour );
}


void
SdlEngine::draw_ship( const yarrr::Ship& ship )
{
  draw_rectangle(
      ship.coordinate.x - 800,
      ship.coordinate.y - 800,
      4, 0x00ff00 );
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

