#include "sdl_engine.hpp"

#include <yarrr/physical_parameters.hpp>

#include <cmath>
#include <cassert>
#include <SDL2/SDL.h>
#include <iostream>
#include <algorithm>

#include <thectci/service_registry.hpp>
#include <thelog/logger.hpp>

namespace
{
  SDL_Color to_sdl_colour( const yarrr::Colour& colour )
  {
    return {
      colour.red,
      colour.green,
      colour.blue,
      colour.alpha
    };
  }
}

TtfInitializer::TtfInitializer()
{
  TTF_Init();
}

TtfInitializer::~TtfInitializer()
{
  TTF_Quit();
}

Font::Font( const std::string& path )
  : font( TTF_OpenFont( path.c_str(), 14 ) )
{
  assert( font );
}

Font::~Font()
{
  TTF_CloseFont( font );
}

SdlEngine::SdlEngine( int16_t x, int16_t y )
  : m_window( nullptr )
  , m_screen_resolution( x, y )
  , m_center_of_screen( x / 2, y / 2 )
  , m_center_in_metres( m_screen_resolution * 0.5 )
  , m_ttf_initializer()
  , m_font( "stuff.ttf" )
{
  if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
  {
    thelog( 1 )( SDL_GetError() );
    assert( false );
  }

  m_window = SDL_CreateWindow(
      "yarrr",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      x, y,
      SDL_WINDOW_SHOWN );
  assert( m_window );
  m_renderer = SDL_CreateRenderer(
      m_window,
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  assert( m_renderer );

  assert( 0 == SDL_SetRenderDrawBlendMode(
        m_renderer,
        SDL_BLENDMODE_BLEND ) );
}


SdlEngine::~SdlEngine()
{
  SDL_DestroyRenderer( m_renderer );
  SDL_DestroyWindow( m_window );
  SDL_Quit();
}


void
SdlEngine::update_screen()
{
  draw_background();
  draw_grid();
  draw_objects();
  SDL_RenderPresent( m_renderer );
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
      draw_scaled_point( yarrr::metres_to_huplons( grid_point ), 2, white );
    }
  }
}


void
SdlEngine::draw_scaled_point(
    const yarrr::Coordinate& coordinate,
    int size,
    const yarrr::Colour& colour )
{
  const yarrr::Coordinate scaled( scale_coordinate( coordinate ) );
  //todo: scale size as well
  draw_point( scaled.x, scaled.y, size, colour );
}

void
SdlEngine::set_colour( const yarrr::Colour& colour )
{
  SDL_SetRenderDrawColor( m_renderer, colour.red, colour.green, colour.blue, colour.alpha );
}

void
SdlEngine::draw_scaled_line(
    const yarrr::Coordinate& start,
    const yarrr::Coordinate& end,
    const yarrr::Colour& colour )
{
  const yarrr::Coordinate scaled_start( scale_coordinate( start ) );
  const yarrr::Coordinate scaled_end( scale_coordinate( end ) );

  set_colour( colour );
  SDL_RenderDrawLine( m_renderer, scaled_start.x, scaled_start.y, scaled_end.x, scaled_end.y );
}

void
SdlEngine::draw_point(
    int16_t x,
    int16_t y,
    int size, const yarrr::Colour& colour )
{
  set_colour( colour );
  SDL_Rect rectangle = {
    static_cast<Sint16>( x ),
    static_cast<Sint16>( y ),
    static_cast<Uint16>( size ),
    static_cast<Uint16>( size ) };

  SDL_RenderFillRect( m_renderer, &rectangle );
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
SdlEngine::draw_laser( const yarrr::PhysicalParameters& laser )
{
  const yarrr::Coordinate head( yarrr::heading( laser, 60 ) );
  if ( !is_on_screen( laser.coordinate ) )
  {
    return;
  }

  draw_scaled_line( laser.coordinate, laser.coordinate - head, strange );
}

void
SdlEngine::draw_particle( const yarrr::PhysicalParameters& particle, uint64_t age )
{
  if ( !is_on_screen( particle.coordinate ) )
  {
    return;
  }

  yarrr::Colour particle_colour( white );
  particle_colour.alpha = 255.0 / 3100000.0 * ( 3100000 - age );
  draw_scaled_point( particle.coordinate, 4, particle_colour );
}

void
SdlEngine::draw_ship( const yarrr::PhysicalParameters& ship )
{
  if ( !is_on_screen( ship.coordinate ) )
  {
    const yarrr::Coordinate diff(
        ( yarrr::huplons_to_metres( ship.coordinate ) - m_center_in_metres ) * 0.01 +
          m_center_of_screen );
    draw_point( diff.x, diff.y, 4, white );
    return;
  }

  const yarrr::Coordinate head( yarrr::heading( ship, 60 ) );
  const yarrr::Coordinate perpendicular( yarrr::perpendicular( head ) );
  draw_scaled_point( ship.coordinate, 4, strange );
  draw_scaled_point( ship.coordinate + head, 4, green );
  draw_scaled_point( ship.coordinate - head, 4, red );
  draw_scaled_point( ship.coordinate - head * 0.5 + perpendicular * 0.5, 4, red );
  draw_scaled_point( ship.coordinate - head * 0.5 - perpendicular * 0.5, 4, red );
}


void
SdlEngine::print_text( uint16_t x, uint16_t y, const std::string& message, const yarrr::Colour& colour )
{
  SDL_Surface *surface(
      TTF_RenderText_Blended(
        m_font.font,
        message.c_str(),
        to_sdl_colour( colour ) ) );
  assert( surface );
  SDL_Texture *texture( SDL_CreateTextureFromSurface( m_renderer, surface ) );
  assert( texture );
  SDL_FreeSurface( surface );

  SDL_Rect destination{ x, y, 0, 0 };
  SDL_QueryTexture( texture, nullptr, nullptr, &destination.w, &destination.h );

  assert( 0 == SDL_RenderCopy( m_renderer, texture, nullptr, &destination ) );
  SDL_DestroyTexture( texture );
}

void
SdlEngine::print_text_tokens( uint16_t x, uint16_t y, const yarrr::TextTokens& tokens )
{
  size_t length( 0 );
  for ( const auto& token : tokens )
  {
    print_text( x + length * 7, y, token.text, token.colour );
    length += token.text.length();
  }
}

void
SdlEngine::draw_background()
{
  SDL_SetRenderDrawColor( m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
  SDL_RenderClear( m_renderer );
}

namespace
{
  the::ctci::AutoServiceRegister< yarrr::GraphicalEngine, SdlEngine > auto_sdl_engine_register( 800, 600 );
}

