#pragma once
#include <cstdint>
#include <vector>
#include <yarrr/graphical_engine.hpp>
#include <thectci/id.hpp>
#include <SDL2/SDL_ttf.h>

struct SDL_Surface;
struct SDL_Renderer;
struct SDL_Window;

namespace yarrr
{
  class PhysicalParameters;
}

class TtfInitializer
{
  public:
    TtfInitializer();
    ~TtfInitializer();
};

class Colour
{
  public:
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t alpha;
};

class Font
{
  public:
    Font( const std::string& path );
    ~Font();

    TTF_Font * font;
};

class SdlEngine : public yarrr::GraphicalEngine
{
  public:
    add_ctci( "sdl_engine" );
    SdlEngine( int16_t x, int16_t y );
    ~SdlEngine();

    SdlEngine( const SdlEngine& ) = delete;
    SdlEngine& operator=( const SdlEngine& ) = delete;

    virtual void focus_to( const yarrr::Coordinate& center ) override;
    virtual void draw_ship( const yarrr::PhysicalParameters& ship ) override;
    virtual void print_text( uint16_t x, uint16_t y, const std::string& ) override;

    void update_screen();

  private:
    void draw_point(
        int16_t x,
        int16_t y,
        int size, const Colour& );

    void draw_scaled_point(
        const yarrr::Coordinate&,
        int size,
        const Colour& );

    yarrr::Coordinate scale_coordinate( const yarrr::Coordinate& ) const;

    bool is_on_screen( const yarrr::Coordinate& ) const;

    void draw_grid();
    void draw_background();

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    const yarrr::Coordinate m_screen_resolution;
    const yarrr::Coordinate m_center_of_screen;

    yarrr::Coordinate m_center_in_metres;

    const Colour white{ 255, 255, 255, 255 };
    const Colour red{ 255, 0, 0, 255 };
    const Colour green{ 0, 255, 0, 255 };
    const Colour strange{ 255, 255, 0, 255 };

    TtfInitializer m_ttf_initializer;
    Font m_font;
};

