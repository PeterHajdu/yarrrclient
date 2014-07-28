#pragma once
#include <cstdint>
#include <vector>
#include <yarrr/graphical_engine.hpp>
#include <thectci/id.hpp>

struct SDL_Surface;
struct SDL_Window;

namespace yarrr
{
  class PhysicalParameters;
}

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

    void update_screen();

  private:
    void draw_point(
        int16_t x,
        int16_t y,
        int size, uint32_t colour );

    void draw_scaled_point(
        const yarrr::Coordinate&,
        int size,
        uint32_t colour );

    yarrr::Coordinate scale_coordinate( const yarrr::Coordinate& ) const;

    bool is_on_screen( const yarrr::Coordinate& ) const;

    void draw_grid();
    void draw_background();

    SDL_Window* m_window;
    SDL_Surface* m_screen;

    const yarrr::Coordinate m_screen_resolution;
    const yarrr::Coordinate m_center_of_screen;

    yarrr::Coordinate m_center_in_metres;
};

