#pragma once

#include "text_token.hpp"
#include <theui/window.hpp>
#include <theui/text_box.hpp>
#include <yarrr/graphical_engine.hpp>

namespace yarrr
{

class GraphicalEngine;

}

namespace yarrrc
{

class Window :
  public the::ui::Window,
  public yarrr::GraphicalObject
{
  public:
    Window(
        yarrr::GraphicalEngine& graphical_engine,
        const the::ui::Window::Coordinate& coordinate,
        const the::ui::Size& size,
        the::ui::Window::Restructure window_restructure = the::ui::do_nothing_window_restructure );

  private:
    virtual void draw() const override;
};

class TextBox :
  public the::ui::TextBox< TextToken >,
  public yarrr::GraphicalObject
{
  public:
    TextBox(
        const TextToken::Container& content,
        yarrr::GraphicalEngine& graphical_engine,
        const the::ui::Window::Coordinate& coordinate,
        const the::ui::Size& size );

  private:
    virtual void draw() const override;

};

}

