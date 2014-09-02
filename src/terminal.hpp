#pragma once

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

class Terminal : public yarrr::GraphicalObject, public the::ctci::Dispatcher
{
  public:
    Terminal( yarrr::GraphicalEngine&, int number_of_messages );
    virtual void draw() const override;

    void home();
    void end();
    void scroll_up();
    void scroll_down();

  private:
    void handle_chat_message( const yarrr::ChatMessage& );
    void normalize_first_index();

    void jump_to_last_page();
    std::vector< yarrr::TextTokens > m_messages;
    const int m_number_of_shown_messages;
    const int m_lines_to_scroll;
    int m_first_message_index;
};

}

