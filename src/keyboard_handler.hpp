#pragma once
#include <yarrr/command.hpp>
#include <yarrr/cli.hpp>
#include <thectci/multiplexer.hpp>

class KeyboardHandler : public the::ctci::Multiplexer
{
  public:
    KeyboardHandler( bool& running );
    void check_keyboard( the::time::Time& now );
  private:
    void send_command( yarrr::Command::Type cmd, the::time::Time& timestamp );
    void handle_text_input();

    bool& m_running;
    yarrr::Cli m_cli;
};

