#include "../src/mission_window.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/mission_container.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_mission_window )
{

  void update_mission( yarrr::TaskState state )
  {
    mission = yarrr::Mission( info );
    objective_state = state;
    mission.add_objective( objective );
    mission.update();
    assert( mission.state() == objective_state );
    mission_source->dispatch( mission );
    graphical_engine.draw_objects();
  }

  void SetUp()
  {
    graphical_engine.printed_texts.clear();
    mission_source.reset( new the::ctci::Dispatcher() );
    mission_window.reset( new yarrrc::MissionWindow( graphical_engine, *mission_source ) );
  }

  It( prints_out_mission_name )
  {
    update_mission( yarrr::ongoing );
    AssertThat( graphical_engine.was_printed( mission_name ), Equals( true ) );
  }

  It( prints_out_mission_description )
  {
    update_mission( yarrr::ongoing );
    AssertThat( graphical_engine.was_printed( mission_description ), Equals( true ) );
  }

  It( prints_out_objectives )
  {
    update_mission( yarrr::ongoing );
    AssertThat( graphical_engine.was_printed( objective_description ), Equals( true ) );
  }

  It( should_keep_a_history_of_finished_missions )
  {
    update_mission( yarrr::succeeded );
    AssertThat( graphical_engine.was_printed( mission_name ), Equals( true ) );
    AssertThat( graphical_engine.was_printed( "Mission log" ), Equals( true ) );
  }

  test::GraphicalEngine graphical_engine;
  std::unique_ptr< yarrrc::MissionWindow > mission_window;
  std::unique_ptr< the::ctci::Dispatcher > mission_source;

  const std::string mission_name{ "mission name" };
  const std::string mission_description{ "mission description" };
  const yarrr::Mission::Info info{ mission_name, mission_description };
  yarrr::Mission mission{ info };
  const std::string objective_description{ "objective description" };
  yarrr::TaskState objective_state;
  const yarrr::Mission::Objective objective{ objective_description,
    [ this ]( const std::string& ) -> yarrr::TaskState
    {
      return objective_state;
    } };
};

