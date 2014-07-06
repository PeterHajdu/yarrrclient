#pragma once
#include <cstdint>
#include <vector>
#include <yarrr/types.hpp>

struct SDL_Surface;
struct SDL_Window;

namespace yarrr
{
  class Object;
}

class DrawableObject;
class SdlEngine
{
  public:
    SdlEngine( int32_t x, int32_t y );
    ~SdlEngine();

    SdlEngine( const SdlEngine& ) = delete;
    SdlEngine& operator=( const SdlEngine& ) = delete;

    void focus_to( const yarrr::Coordinate& center );
    void draw_ship( const yarrr::Object& ship );

    void draw_point(
        int32_t x,
        int32_t y,
        int size, uint32_t colour );

    void update_screen();
    void register_object( DrawableObject& object );

  private:
    bool is_on_screen( int32_t x, int32_t y ) const;

    void draw_objects();
    void draw_background();

    SDL_Window* m_window;
    SDL_Surface* m_screen;

    const int32_t m_x;
    const int32_t m_y;

    std::vector< DrawableObject* > m_objects;

    yarrr::Coordinate m_center;
};

class DrawableObject
{
  public:
    DrawableObject( SdlEngine& graphical_engine )
      : m_graphical_engine( graphical_engine )
    {
      m_graphical_engine.register_object( *this );
    }

    virtual void draw() = 0;

  protected:
    SdlEngine& m_graphical_engine;
};

