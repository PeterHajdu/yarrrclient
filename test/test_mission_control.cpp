#include "../src/mission_control.hpp"
#include "../src/local_event_dispatcher.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/mission_container.hpp>
#include <igloo/igloo_alt.h>
#include <thectci/service_registry.hpp>

using namespace igloo;

Describe( a_mission_control )
{

  void update_mission( yarrr::TaskState state )
  {
    mission = yarrr::Mission( info );
    mission_id = mission.id();
    finished_mission_id = 0;
    objective_state = state;
    mission.add_objective( objective );
    mission.update();
    was_mission_updated = false;
    mission_source->dispatch( mission );
    graphical_engine.draw_objects();
  }

  void SetUp()
  {
    graphical_engine.printed_texts.clear();
    mission_source.reset( new the::ctci::Dispatcher() );
    mission_control.reset( new yarrrc::MissionControl( graphical_engine, *mission_source ) );

    was_mission_finished_sent_out = false;

    auto& dispatcher( the::ctci::service< LocalEventDispatcher >().dispatcher );
    dispatcher.register_listener< yarrrc::MissionFinished >(
        [ this ]( const yarrrc::MissionFinished& finished )
        {
          was_mission_finished_sent_out = true;
          finished_mission_id = finished.mission.id();
        } );
  }

  void TearDown()
  {
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

  It( can_update_missions )
  {
    update_mission( yarrr::ongoing );
    mission_control->update();
    AssertThat( was_mission_updated, Equals( true ) );
  }

  It( should_send_a_mission_finished_event_when_a_mission_finishes )
  {
    update_mission( yarrr::failed );
    AssertThat( was_mission_finished_sent_out, Equals( true ) );
    AssertThat( finished_mission_id, Equals( mission_id ) );
  }

  test::GraphicalEngine graphical_engine;
  std::unique_ptr< yarrrc::MissionControl > mission_control;
  std::unique_ptr< the::ctci::Dispatcher > mission_source;

  yarrr::Mission::Id mission_id;
  const std::string mission_name{ "mission name" };
  const std::string mission_description{ "mission description" };
  const yarrr::Mission::Info info{ mission_name, mission_description };
  yarrr::Mission mission{ info };
  const std::string objective_description{ "objective description" };
  yarrr::TaskState objective_state;
  bool was_mission_updated;
  const yarrr::Mission::Objective objective{ objective_description,
    [ this ]( const std::string& ) -> yarrr::TaskState
    {
      was_mission_updated = true;
      return objective_state;
    } };
  bool was_mission_finished_sent_out;
  yarrr::Mission::Id finished_mission_id;
};

