#pragma once
#include <cstdint>
#include <vector>
#include <memory>
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

class Font
{
  public:
    Font( const std::string& path );
    ~Font();

    TTF_Font * font;
};

class Window;
class Renderer
{
  public:
    typedef std::unique_ptr< Renderer > Pointer;
    Renderer( SDL_Window* );
    ~Renderer();
    operator SDL_Renderer*();

  private:
    SDL_Renderer* m_renderer;
};

class Window
{
  public:
    Window();
    ~Window();
    Renderer::Pointer create_renderer();

    const yarrr::Coordinate screen_resolution;

  private:
    SDL_Window* m_window;
};


class SdlInitializer
{
  public:
    SdlInitializer();
    ~SdlInitializer();
};

class SdlEngine : public yarrr::GraphicalEngine
{
  public:
    add_ctci( "sdl_engine" );
    SdlEngine();
    ~SdlEngine();

    SdlEngine( const SdlEngine& ) = delete;
    SdlEngine& operator=( const SdlEngine& ) = delete;

    virtual void draw_loot( const yarrr::PhysicalParameters& ) override;
    virtual void draw_particle( const yarrr::PhysicalParameters&, uint64_t age ) override;
    virtual void focus_to( const yarrr::Coordinate& center ) override;
    virtual void draw_ship( const yarrr::PhysicalParameters& ) override;
    virtual void draw_laser( const yarrr::PhysicalParameters& ) override;
    virtual void print_text( uint16_t x, uint16_t y, const std::string&, const yarrr::Colour& ) override;
    virtual void print_text_tokens( uint16_t x, uint16_t y, const yarrr::TextTokens& ) override;

    void update_screen();

  private:
    void show_on_radar( const yarrr::Coordinate& );

    void draw_point(
        int16_t x,
        int16_t y,
        int size, const yarrr::Colour& );

    void set_colour( const yarrr::Colour& );

    void draw_scaled_line(
        const yarrr::Coordinate&,
        const yarrr::Coordinate&,
        const yarrr::Colour& );

    void draw_scaled_point(
        const yarrr::Coordinate&,
        int size,
        const yarrr::Colour& );

    yarrr::Coordinate scale_coordinate( const yarrr::Coordinate& ) const;

    bool is_on_screen( const yarrr::Coordinate& ) const;

    void draw_grid();
    void draw_background();

    SdlInitializer m_sdl_initializer;
    TtfInitializer m_ttf_initializer;
    Window m_window;
    Renderer::Pointer m_renderer;

    const yarrr::Coordinate m_screen_resolution;
    const yarrr::Coordinate m_center_of_screen;

    yarrr::Coordinate m_center_in_metres;

    const yarrr::Colour white{ 255, 255, 255, 255 };
    const yarrr::Colour red{ 255, 0, 0, 255 };
    const yarrr::Colour green{ 0, 255, 0, 255 };
    const yarrr::Colour strange{ 255, 255, 0, 255 };

    Font m_font;
};

