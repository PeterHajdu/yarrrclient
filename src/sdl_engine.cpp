#include "sdl_engine.hpp"
#include "resources.hpp"

#include <yarrr/physical_parameters.hpp>
#include <yarrr/object.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/shape.hpp>
#include <yarrr/shape_behavior.hpp>

#include <cmath>
#include <cassert>
#include <SDL2/SDL.h>
#include <algorithm>

#include <thectci/service_registry.hpp>
#include <yarrr/log.hpp>
#include <theconf/configuration.hpp>

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

  void handle_unrecoverable_sdl_error()
  {
    thelog( yarrr::log::error )( SDL_GetError() );
    assert( false );
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

SdlInitializer::SdlInitializer()
{
  if ( SDL_Init( SDL_INIT_VIDEO ) != 0 )
  {
    handle_unrecoverable_sdl_error();
  }
}

SdlInitializer::~SdlInitializer()
{
  SDL_Quit();
}

Font::Font( const std::string& path )
  : font( TTF_OpenFont( path.c_str(), 14 ) )
{
  if ( nullptr == font )
  {
    handle_unrecoverable_sdl_error();
  }
}

Font::~Font()
{
  TTF_CloseFont( font );
}

namespace
{

  uint32_t calculate_window_flags()
  {
    uint32_t window_flags( SDL_WINDOW_SHOWN );

    if ( the::conf::has( "fullscreen" ) )
    {
      window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    return window_flags;
  }

  yarrr::Coordinate calculate_screen_resolution()
  {
    yarrr::Coordinate resolution( 800, 600 );

    if ( the::conf::has( "fullscreen" ) )
    {
      const int display_in_use( 0 );
      SDL_DisplayMode current_display_mode;
      assert( 0 == SDL_GetCurrentDisplayMode( display_in_use, &current_display_mode ) );
      resolution = yarrr::Coordinate{ current_display_mode.w, current_display_mode.h };
    }

    return resolution;
  }
}

Window::Window()
  : screen_resolution( calculate_screen_resolution() )
  , m_window( SDL_CreateWindow(
      "yarrr",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      screen_resolution.x, screen_resolution.y,
      calculate_window_flags() ) )
{
  assert( m_window );
}

Window::~Window()
{
  SDL_DestroyWindow( m_window );
}

Renderer::Pointer
Window::create_renderer()
{
  return Renderer::Pointer( new Renderer( m_window ) );
}

Renderer::Renderer( SDL_Window* window )
  : m_renderer(
      SDL_CreateRenderer(
      window,
      -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC ) )
{
  assert( m_renderer );
  assert( 0 == SDL_SetRenderDrawBlendMode(
        m_renderer,
        SDL_BLENDMODE_BLEND ) );
}

Renderer::~Renderer()
{
  SDL_DestroyRenderer( m_renderer );
}

Renderer::operator SDL_Renderer*()
{
  return m_renderer;
}

SdlEngine::SdlEngine()
  : m_sdl_initializer()
  , m_ttf_initializer()
  , m_window()
  , m_renderer( m_window.create_renderer() )
  , m_screen_resolution( m_window.screen_resolution )
  , m_center_of_screen( m_screen_resolution * 0.5 )
  , m_center_of_radar( m_screen_resolution * 0.8 )
  , m_center_in_metres( m_screen_resolution * 0.5 )
  , m_font( the::ctci::service< yarrr::ResourceFinder >().find( "stuff.ttf" ) )
{
}

SdlEngine::~SdlEngine()
{
}


void
SdlEngine::update_screen()
{
  draw_background();
  draw_grid();
  draw_objects();
  draw_radar();
  SDL_RenderPresent( *m_renderer );
}

void
SdlEngine::draw_radar()
{
  const int diff_from_middle( 30 );
  set_colour( { 100, 100, 100, 255 } );
  SDL_RenderDrawLine( *m_renderer,
      m_center_of_radar.x - diff_from_middle, m_center_of_radar.y,
      m_center_of_radar.x + diff_from_middle, m_center_of_radar.y );

  SDL_RenderDrawLine( *m_renderer,
      m_center_of_radar.x, m_center_of_radar.y - diff_from_middle,
      m_center_of_radar.x, m_center_of_radar.y + diff_from_middle );
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
      draw_scaled_point( yarrr::metres_to_huplons( grid_point ), 2, yarrr::Colour::White );
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
  SDL_SetRenderDrawColor( *m_renderer, colour.red, colour.green, colour.blue, colour.alpha );
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
  SDL_RenderDrawLine( *m_renderer, scaled_start.x, scaled_start.y, scaled_end.x, scaled_end.y );
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

  SDL_RenderFillRect( *m_renderer, &rectangle );
}


void
SdlEngine::focus_to( const yarrr::Coordinate& center )
{
   m_center_in_metres = yarrr::huplons_to_metres( center );
}

yarrr::Coordinate
SdlEngine::scale_coordinate( const yarrr::Coordinate& coordinate ) const
{
  yarrr::Coordinate scaled( yarrr::huplons_to_metres( coordinate ) );
  scaled -= m_center_in_metres;
  scaled.y *= -1;
  scaled += m_center_of_screen;
  return scaled;
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
  const yarrr::Coordinate head( yarrr::heading( laser, 10_metres ) );
  if ( !is_on_screen( laser.coordinate ) )
  {
    return;
  }

  draw_scaled_line( laser.coordinate, laser.coordinate - head, yarrr::Colour::Strange );
}

void
SdlEngine::draw_particle( const yarrr::PhysicalParameters& particle, uint64_t age )
{
  if ( !is_on_screen( particle.coordinate ) )
  {
    return;
  }

  yarrr::Colour particle_colour( yarrr::Colour::White );
  particle_colour.alpha = 100.0 / 3100000.0 * ( 3100000 - age );
  draw_scaled_point( particle.coordinate, 3, particle_colour );
}

void
SdlEngine::show_on_radar( const yarrr::Coordinate& coordinate )
{
  yarrr::Coordinate diff( yarrr::huplons_to_metres( coordinate ) - m_center_in_metres );
  diff.y *= -1;
  diff *= 0.01;
  diff += m_center_of_radar;
  draw_point( diff.x, diff.y, 4, yarrr::Colour::White );
}

void
SdlEngine::draw_object_with_shape( const yarrr::Object& object )
{
  const yarrr::PhysicalParameters& parameters(
      yarrr::component_of< yarrr::PhysicalBehavior >( object ).physical_parameters );

  if ( !is_on_screen( parameters.coordinate ) )
  {
    show_on_radar( parameters.coordinate );
    return;
  }



  const yarrr::Shape& shape(
      yarrr::component_of< yarrr::ShapeBehavior >( object ).shape );

  for ( const auto& tile : shape.tiles() )
  {
    draw_tile( parameters.coordinate, parameters.orientation, shape, tile );
  }
}


void
SdlEngine::draw_tile(
    const yarrr::Coordinate& center,
    const yarrr::Angle orientation,
    const yarrr::Shape& shape,
    const yarrr::Tile& tile )
{
  const yarrr::Polygon polygon( yarrr::generate_polygon_from( tile, center, shape.center_of_mass(), orientation ) );
  if ( polygon.size() < 2 )
  {
    return;
  }

  yarrr::Coordinate start{ polygon.at( 3 ) };
  for ( size_t i( 0 ); i < polygon.size(); ++i )
  {
    draw_scaled_line( start, polygon.at( i ), yarrr::Colour::White );
    start = polygon.at( i );
  }
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
  SDL_Texture *texture( SDL_CreateTextureFromSurface( *m_renderer, surface ) );
  assert( texture );
  SDL_FreeSurface( surface );

  SDL_Rect destination{ x, y, 0, 0 };
  SDL_QueryTexture( texture, nullptr, nullptr, &destination.w, &destination.h );

  assert( 0 == SDL_RenderCopy( *m_renderer, texture, nullptr, &destination ) );
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
  SDL_SetRenderDrawColor( *m_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE );
  SDL_RenderClear( *m_renderer );
}


const yarrr::Coordinate&
SdlEngine::screen_resolution() const
{
  return m_screen_resolution;
}

