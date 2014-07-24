#include "keyboard_handler.hpp"
#include <SDL2/SDL.h>

KeyboardHandler::KeyboardHandler( bool& running )
  : m_running( running )
{
}

void
KeyboardHandler::send_command( yarrr::Command::Type cmd, the::time::Time& timestamp )
{
  yarrr::Command command( cmd, timestamp );
  dispatch( command );
}

void
KeyboardHandler::check_keyboard( the::time::Time& now )
{
  SDL_PumpEvents();
  const unsigned char *keystates = SDL_GetKeyboardState( nullptr );
  if (keystates[SDL_SCANCODE_RIGHT])
  {
    send_command( yarrr::Command::cw, now );
  }

  if (keystates[SDL_SCANCODE_LEFT])
  {
    send_command( yarrr::Command::ccw, now );
  }

  if (keystates[SDL_SCANCODE_UP])
  {
    send_command( yarrr::Command::thruster, now );
  }

  if (keystates[SDL_SCANCODE_Q])
  {
    m_running = false;
  }
}

