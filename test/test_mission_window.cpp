#include "../src/mission_window.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/mission_container.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_mission_window )
{

  void update_mission()
  {
    mission_source->dispatch( mission );
    graphical_engine.draw_objects();
  }

  void SetUp()
  {
    graphical_engine.printed_texts.clear();
    mission_source.reset( new the::ctci::Dispatcher() );
    mission_window.reset( new yarrrc::MissionWindow( graphical_engine, *mission_source ) );
    mission = yarrr::Mission( info );
    mission.add_objective( objective );
    update_mission();
  }

  It( prints_out_mission_name )
  {
    AssertThat( graphical_engine.was_printed( mission_name ), Equals( true ) );
  }

  It( prints_out_mission_description )
  {
    AssertThat( graphical_engine.was_printed( mission_description ), Equals( true ) );
  }

  It( prints_out_objectives )
  {
    AssertThat( graphical_engine.was_printed( objective_description ), Equals( true ) );
  }

  test::GraphicalEngine graphical_engine;
  std::unique_ptr< yarrrc::MissionWindow > mission_window;
  std::unique_ptr< the::ctci::Dispatcher > mission_source;

  const std::string mission_name{ "mission name" };
  const std::string mission_description{ "mission description" };
  const yarrr::Mission::Info info{ mission_name, mission_description };
  yarrr::Mission mission{ info };
  const std::string objective_description{ "objective description" };
  const yarrr::Mission::Objective objective{ objective_description, []( const std::string& ) -> yarrr::TaskState { return yarrr::ongoing; } };
};

