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
  class Shape;
  class Tile;
  class Object;
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

    virtual void draw_object_with_shape( const yarrr::Object& ) override;
    virtual void draw_particle( const yarrr::PhysicalParameters&, uint64_t age ) override;
    virtual void focus_to( const yarrr::Coordinate& center ) override;
    virtual void draw_laser( const yarrr::PhysicalParameters& ) override;
    virtual void print_text( uint16_t x, uint16_t y, const std::string&, const yarrr::Colour& ) override;
    virtual void print_text_tokens( uint16_t x, uint16_t y, const yarrr::TextTokens& ) override;
    virtual const yarrr::Coordinate& screen_resolution() const override;

    void update_screen() override;

  private:
    void draw_tile( const yarrr::Coordinate& center, const yarrr::Angle orientation, const yarrr::Shape&, const yarrr::Tile& );
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
    void draw_radar();
    void draw_background();

    SdlInitializer m_sdl_initializer;
    TtfInitializer m_ttf_initializer;
    Window m_window;
    Renderer::Pointer m_renderer;

    const yarrr::Coordinate m_screen_resolution;
    const yarrr::Coordinate m_center_of_screen;
    const yarrr::Coordinate m_center_of_radar;

    yarrr::Coordinate m_center_in_metres;

    Font m_font;
};

