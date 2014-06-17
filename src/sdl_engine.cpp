#include "sdl_engine.hpp"

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
SdlEngine::updateScreen()
{
  drawBackground();
  SDL_UpdateWindowSurface( m_window );
}


void
SdlEngine::drawRawRectangle(
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
SdlEngine::drawBackground()
{
  SDL_Rect rectangle =
  { static_cast<unsigned short>( 0 ), static_cast<unsigned short>( 0 ),
    static_cast<unsigned short>( m_x ), static_cast<unsigned short>( m_y ) };
  SDL_FillRect( m_screen, &rectangle, 0x000000 );
}

