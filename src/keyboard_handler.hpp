#pragma once
#include <yarrr/command.hpp>
#include "cli.hpp"
#include "terminal.hpp"
#include <thectci/dispatcher.hpp>

class KeyboardHandler : public the::ctci::Dispatcher
{
  public:
    KeyboardHandler( bool& running );
    void check_keyboard( the::time::Time& now );
  private:
    void send_command( yarrr::Command::Type cmd, the::time::Time& timestamp );
    void handle_text_input( the::time::Time& now );

    bool& m_running;
    yarrrc::Cli m_cli;
    yarrrc::Terminal m_terminal;
};

