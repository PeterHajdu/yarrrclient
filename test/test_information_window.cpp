#include "../src/information_window.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <thetime/clock.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( the_information_window )
{

  void SetUp()
  {
    graphical_engine.printed_texts.clear();
    information_window.reset( new yarrrc::InformationWindow( graphical_engine, clock ) );
    graphical_engine.draw_objects();
  }

  It( prints_out_universe_time_text )
  {
    AssertThat( graphical_engine.was_printed( "universe time:" ), Equals( true ) );
  }

  It( prints_out_the_universe_time )
  {
    AssertThat( graphical_engine.was_printed( "01:00" ), Equals( true ) );
  }

  test::GraphicalEngine graphical_engine;
  std::unique_ptr< yarrrc::InformationWindow > information_window;
  the::time::Clock clock;
};

