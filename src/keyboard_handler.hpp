#pragma once
#include <yarrr/command.hpp>
#include <yarrr/cli.hpp>
#include <yarrr/terminal.hpp>
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
    yarrr::Cli m_cli;
    yarrr::Terminal m_terminal;
};

