#pragma once
#include <yarrr/command.hpp>
#include <thectci/multiplexer.hpp>

class KeyboardHandler : public the::ctci::Multiplexer
{
  public:
    KeyboardHandler( bool& running );
    void send_command( yarrr::Command::Type cmd, the::time::Time& timestamp );
    void check_keyboard( the::time::Time& now );
  private:
    bool& m_running;
};

