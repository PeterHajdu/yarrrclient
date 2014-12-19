#include "../src/window.hpp"
#include "../src/text_token.hpp"
#include "test_services.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_window )
{

  void SetUp()
  {
    graphical_engine = &test::get_cleaned_up_graphical_engine();
    window = std::make_unique< yarrrc::Window >(
        *graphical_engine,
        top_left,
        size );
    graphical_engine->draw_objects();
  }

  std::unique_ptr< yarrrc::Window > window;
  const the::ui::Size size{ 30, 40 };
  const the::ui::Window::Coordinate top_left{ 10, 20 };
  const the::ui::Window::Coordinate bottom_right{
    top_left.x + size.width,
    top_left.y + size.height };

  test::GraphicalEngine* graphical_engine;
};

Describe( a_text_box )
{

  void SetUp()
  {
    graphical_engine = &test::get_cleaned_up_graphical_engine();
    text_box = std::make_unique< yarrrc::TextBox >(
        initial_content,
        *graphical_engine,
        top_left,
        size );
    graphical_engine->draw_objects();
  }

  It( prints_out_text )
  {
    AssertThat( graphical_engine->was_printed( initial_text.text() ), Equals( true ) );
  }

  test::GraphicalEngine* graphical_engine;
  std::unique_ptr< yarrrc::TextBox > text_box;
  const the::ui::Size size{ 30, 40 };
  const the::ui::Window::Coordinate top_left{ 10, 20 };
  const yarrrc::TextToken initial_text{ "initial text" };
  const yarrrc::TextToken::Container initial_content{ initial_text };
};

