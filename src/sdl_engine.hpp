#pragma once
#include <cstdint>
#include <vector>
#include <yarrr/types.hpp>

struct SDL_Surface;
struct SDL_Window;

namespace yarrr
{
  class PhysicalParameters;
}

class DrawableObject;
class SdlEngine
{
  public:
    SdlEngine( int16_t x, int16_t y );
    ~SdlEngine();

    SdlEngine( const SdlEngine& ) = delete;
    SdlEngine& operator=( const SdlEngine& ) = delete;

    void focus_to( const yarrr::Coordinate& center );
    void draw_ship( const yarrr::PhysicalParameters& ship );

    void update_screen();
    void register_object( DrawableObject& object );
    void delete_object( const DrawableObject& object );

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

    void draw_objects();
    void draw_grid();
    void draw_background();

    SDL_Window* m_window;
    SDL_Surface* m_screen;

    const yarrr::Coordinate m_screen_resolution;
    const yarrr::Coordinate m_center_of_screen;
    std::vector< DrawableObject* > m_objects;

    yarrr::Coordinate m_center_in_metres;
};

class DrawableObject
{
  public:
    DrawableObject( SdlEngine& graphical_engine )
      : m_graphical_engine( graphical_engine )
    {
      m_graphical_engine.register_object( *this );
    }

    virtual ~DrawableObject()
    {
      m_graphical_engine.delete_object( *this );
    }

    virtual void draw() = 0;

  protected:
    SdlEngine& m_graphical_engine;
};

