#include "keyboard_handler.hpp"
#include "local_event_dispatcher.hpp"
#include <thectci/service_registry.hpp>
#include <SDL2/SDL.h>

KeyboardHandler::KeyboardHandler( bool& running )
  : m_running( running )
  , m_cli( 0, 100, the::ctci::service< yarrr::GraphicalEngine >() )
  , m_terminal( the::ctci::service< yarrr::GraphicalEngine >() )
{
  m_cli.register_dispatcher( the::ctci::service< LocalEventDispatcher >().outgoing );
  the::ctci::service< LocalEventDispatcher >().incoming.register_dispatcher( m_terminal );
  SDL_StartTextInput();
}

void
KeyboardHandler::send_command( yarrr::ShipControl::Type cmd, const the::time::Time& timestamp )
{
  yarrr::ShipControl command( cmd, timestamp );
  dispatch( command );
}

void
KeyboardHandler::handle_text_input( const the::time::Time& now )
{
  //todo: split this up somehow
  //todo: should add focus selection and destination of events somehow
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
        else if ( event.key.keysym.sym == SDLK_LCTRL )
        {
          send_command( yarrr::ShipControl::fire, now );
        }
        break;
    }
  }
}

void
KeyboardHandler::check_keyboard( const the::time::Time& now )
{
  handle_text_input( now );
  SDL_PumpEvents();
  const unsigned char *keystates = SDL_GetKeyboardState( nullptr );
  if (keystates[SDL_SCANCODE_RIGHT])
  {
    send_command( yarrr::ShipControl::port_thruster, now );
  }

  if (keystates[SDL_SCANCODE_LEFT])
  {
    send_command( yarrr::ShipControl::starboard_thruster, now );
  }

  if (keystates[SDL_SCANCODE_UP])
  {
    send_command( yarrr::ShipControl::main_thruster, now );
  }
}

