#pragma once
#include <cstdint>
#include <vector>

struct SDL_Surface;
struct SDL_Window;

namespace yarrr
{
  struct Ship;
}

class DrawableObject;
class SdlEngine
{
  public:
    SdlEngine( uint32_t x, uint32_t y );
    ~SdlEngine();

    SdlEngine( const SdlEngine& ) = delete;
    SdlEngine& operator=( const SdlEngine& ) = delete;

    void draw_ship( const yarrr::Ship& ship );

    void draw_rectangle(
        uint32_t x,
        uint32_t y,
        int size, uint32_t colour );

    void update_screen();
    void register_object( DrawableObject& object );

  private:
    void draw_objects();
    void draw_background();

    SDL_Window* m_window;
    SDL_Surface* m_screen;

    const uint32_t m_x;
    const uint32_t m_y;

    std::vector< DrawableObject* > m_objects;
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
