#include "sdl_engine.hpp"

#include <yarrr/physical_parameters.hpp>

#include <cmath>
#include <cassert>
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>

#include <thectci/service_registry.hpp>

SdlEngine::SdlEngine( int16_t x, int16_t y )
  : m_window( nullptr )
  , m_screen( nullptr )
  , m_screen_resolution( x, y )
  , m_center_of_screen( x / 2, y / 2 )
  , m_center_in_metres( m_screen_resolution * 0.5 )
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
  draw_grid();
  draw_objects();
  SDL_UpdateWindowSurface( m_window );
}

void
SdlEngine::draw_grid()
{
  const yarrr::Coordinate top_left( ( m_center_in_metres - m_center_of_screen ) * 0.01 * 100 );
  const yarrr::Vector< int64_t > number_of_dots( m_screen_resolution * 0.01 );
  for ( ssize_t i( 0 ); i <= number_of_dots.x; ++i )
  {
    for ( ssize_t j( 0 ); j <= number_of_dots.y; ++j )
    {
      const yarrr::Coordinate grid_point( top_left + yarrr::Coordinate( i * 100, j * 100 ) );
      draw_scaled_point( yarrr::metres_to_huplons( grid_point ), 2, 0xaaaaaa );
    }
  }
}


void
SdlEngine::draw_scaled_point(
    const yarrr::Coordinate& coordinate,
    int size,
    uint32_t colour )
{
  const yarrr::Coordinate scaled( scale_coordinate( coordinate ) );
  //todo: scale size as well
  draw_point( scaled.x, scaled.y, size, colour );
}


void
SdlEngine::draw_point(
    int16_t x,
    int16_t y,
    int size, uint32_t colour )
{
  SDL_Rect rectangle = {
    static_cast<Sint16>( x ),
    static_cast<Sint16>( y ),
    static_cast<Uint16>( size ),
    static_cast<Uint16>( size ) };

  SDL_FillRect( m_screen, &rectangle, colour );
}


void
SdlEngine::focus_to( const yarrr::Coordinate& center )
{
   m_center_in_metres = yarrr::huplons_to_metres( center );
}

yarrr::Coordinate
SdlEngine::scale_coordinate( const yarrr::Coordinate& coordinate ) const
{
  return yarrr::huplons_to_metres( coordinate ) - m_center_in_metres + m_center_of_screen;
}

bool
SdlEngine::is_on_screen( const yarrr::Coordinate& coordinate ) const
{
  const yarrr::Coordinate scaled( scale_coordinate( coordinate ) );
  return
    abs( scaled.x - m_center_of_screen.x ) < m_center_of_screen.x &&
    abs( scaled.y - m_center_of_screen.y ) < m_center_of_screen.y;
}

void
SdlEngine::draw_ship( const yarrr::PhysicalParameters& ship )
{
  const yarrr::Coordinate heading(
      cos( ship.angle * 3.14 / 180.0 / 4.0 ) * 60.0,
      sin( ship.angle * 3.14 / 180.0 / 4.0 ) * 60.0 );

  if ( !is_on_screen( ship.coordinate ) )
  {
    const yarrr::Coordinate diff(
        ( yarrr::huplons_to_metres( ship.coordinate ) - m_center_in_metres ) * 0.01 +
          m_center_of_screen );
    draw_point( diff.x, diff.y, 4, 0xffffff );
    return;
  }

  const yarrr::Coordinate perpendicular( yarrr::perpendicular( heading ) );
  draw_scaled_point( ship.coordinate, 4, 0xaaaa00 );
  draw_scaled_point( ship.coordinate + heading, 4, 0x00ff00 );
  draw_scaled_point( ship.coordinate - heading, 4, 0xff0000 );
  draw_scaled_point( ship.coordinate - heading * 0.5 + perpendicular * 0.5, 4, 0xff0000 );
  draw_scaled_point( ship.coordinate - heading * 0.5 - perpendicular * 0.5, 4, 0xff0000 );
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
  SDL_Rect rectangle{
    static_cast<unsigned short>( 0 ),
    static_cast<unsigned short>( 0 ),
    static_cast<unsigned short>( m_screen_resolution.x ),
    static_cast<unsigned short>( m_screen_resolution.y ) };
  SDL_FillRect( m_screen, &rectangle, 0x000000 );
}


void
SdlEngine::register_object( DrawableObject& object )
{
  m_objects.push_back( &object );
}


void
SdlEngine::delete_object( const DrawableObject& object )
{
  m_objects.erase(
      std::remove( std::begin( m_objects ), std::end( m_objects ), &object )
      , std::end( m_objects ) );
}

namespace
{
  the::ctci::AutoServiceRegister< SdlEngine, SdlEngine > auto_sdl_engine_register( 800, 600 );
}

