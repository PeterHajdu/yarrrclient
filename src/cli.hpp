#pragma once

#include <yarrr/graphical_engine.hpp>
#include <thectci/dispatcher.hpp>

namespace yarrrc
{

class Cli :
  public yarrr::GraphicalObject,
  public the::ctci::Dispatcher
{
  public:
    Cli( int x, int y, yarrr::GraphicalEngine& );
    virtual ~Cli() = default;

    virtual void draw() const override;

    void append( const std::string& );
    void backspace();
    void finalize();

  private:
    std::string m_text;
    const int m_x;
    const int m_y;

    static const std::string m_prompt;
};

}

