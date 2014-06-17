#pragma once
#include <cstdint>

struct SDL_Surface;
struct SDL_Window;

class SdlEngine
{
  public:
    SdlEngine( uint32_t x, uint32_t y );
    ~SdlEngine();

    SdlEngine( const SdlEngine& ) = delete;
    SdlEngine& operator=( const SdlEngine& ) = delete;

    virtual void drawRawRectangle(
        uint32_t x,
        uint32_t y,
        int size, uint32_t colour );

    void updateScreen();

  private:
    void drawBackground();
    SDL_Window* m_window;
    SDL_Surface* m_screen;

    uint32_t m_x;
    uint32_t m_y;
};

