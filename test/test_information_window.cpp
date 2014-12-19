#include "../src/information_window.hpp"
#include "../src/local_event_dispatcher.hpp"
#include "../src/wakeup.hpp"
#include "test_services.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( the_information_window )
{
  void update_time()
  {
    the::ctci::service< LocalEventDispatcher >().wakeup.dispatch(
        yarrrc::EverySecond( 0u ) );
  }

  void SetUp()
  {
    graphical_engine = &test::get_cleaned_up_graphical_engine();
    information_window.reset( new yarrrc::InformationWindow( *graphical_engine ) );
    update_time();
    graphical_engine->draw_objects();
  }

  void TearDown()
  {
    the::ctci::service< LocalEventDispatcher >().wakeup.clear();
  }

  It( prints_out_universe_time_text )
  {
    AssertThat( graphical_engine->was_printed( "universe time:" ), Equals( true ) );
  }

  It( prints_out_the_universe_time )
  {
    AssertThat( graphical_engine->was_printed( "00:00" ), Equals( true ) );
  }

  test::GraphicalEngine* graphical_engine;
  std::unique_ptr< yarrrc::InformationWindow > information_window;
};

