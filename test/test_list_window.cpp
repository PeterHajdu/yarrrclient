#include "../src/list_window.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_list_window )
{

  void SetUp()
  {
    graphical_engine.printed_texts.clear();
    list_window.reset( new yarrrc::ListWindow( 0, 0, graphical_engine,
          [ this ]() -> yarrrc::ListWindow::Lines
          {
            return lines;
          } ) );
    assert( lines.size() );
    graphical_engine.draw_objects();
  }

  It( prints_out_lines_provided_by_the_generator )
  {
    AssertThat( graphical_engine.was_printed( lines.front().text ), Equals( true ) );
    AssertThat( graphical_engine.was_printed( lines.back().text ), Equals( true ) );
  }

  test::GraphicalEngine graphical_engine;
  std::unique_ptr< yarrrc::ListWindow > list_window;
  const yarrrc::ListWindow::Lines lines{ { "first line", yarrr::Colour::White }, { "second line", yarrr::Colour::White } };
};
