#include "keyboard_handler.hpp"
#include <thectci/service_registry.hpp>
#include <SDL2/SDL.h>

KeyboardHandler::KeyboardHandler( bool& running )
  : m_running( running )
  , m_cli( 0, 100, the::ctci::service< yarrr::GraphicalEngine >() )
{
  m_cli.register_multiplexer( *this );
  SDL_StartTextInput();
}

void
KeyboardHandler::send_command( yarrr::Command::Type cmd, the::time::Time& timestamp )
{
  yarrr::Command command( cmd, timestamp );
  dispatch( command );
}

void
KeyboardHandler::handle_text_input()
{
  //todo: split this up somehow
  SDL_Event event;
  while( SDL_PollEvent( &event ) )
  {
    switch( event.type )
    {
      case SDL_TEXTINPUT:
        m_cli.append( event.text.text );
        break;
      case SDL_KEYDOWN:
        if ( event.key.keysym.sym == SDLK_RETURN )
        {
          m_cli.finalize();
        }
        else if ( event.key.keysym.sym == SDLK_ESCAPE )
        {
          m_running = false;
        }
        else if ( event.key.keysym.sym == SDLK_BACKSPACE )
        {
          m_cli.backspace();
        }
        break;
    }
  }
}

void
KeyboardHandler::check_keyboard( the::time::Time& now )
{
  handle_text_input();
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
}

