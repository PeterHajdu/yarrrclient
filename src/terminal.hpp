#pragma once

#include "window.hpp"
#include "text_token.hpp"
#include <yarrr/graphical_engine.hpp>
#include <vector>
#include <string>
#include <thectci/dispatcher.hpp>

namespace yarrr
{
class ChatMessage;
class Command;
}

namespace yarrrc
{

class Terminal : public the::ctci::Dispatcher
{
  public:
    Terminal( yarrr::GraphicalEngine& );

  private:
    void handle_chat_message( const yarrr::ChatMessage& );
    void handle_error_message( const yarrr::Command& );

    void add_line( TextToken::Container );
    Window m_window;
    yarrr::GraphicalEngine& m_graphical_engine;
};

}

