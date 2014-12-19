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
}

namespace yarrrc
{

class Terminal : public the::ctci::Dispatcher
{
  public:
    Terminal( yarrr::GraphicalEngine&, int number_of_messages );

    void home();
    void end();
    void scroll_up();
    void scroll_down();

  private:
    void update_window();
    void handle_chat_message( const yarrr::ChatMessage& );
    void normalize_first_index();

    void jump_to_last_page();

    using Line = yarrrc::TextToken::Container;
    using Lines = std::vector< Line >;
    Lines m_messages;
    const int m_number_of_shown_messages;
    const int m_lines_to_scroll;
    int m_first_message_index;

    Window m_window;
    yarrr::GraphicalEngine& m_graphical_engine;
};

}

