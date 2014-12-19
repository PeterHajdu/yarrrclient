#pragma once

#include "window.hpp"
#include <yarrr/graphical_engine.hpp>
#include <thectci/dispatcher.hpp>

namespace yarrrc
{

class Cli : public the::ctci::Dispatcher
{
  public:
    Cli( int x, int y, yarrr::GraphicalEngine& );
    virtual ~Cli() = default;

    void append( const std::string& );
    void backspace();
    void finalize();

  private:
    void update_text_box();

    std::string m_text;
    TextBox m_text_box;

    static const std::string m_prompt;
};

}

