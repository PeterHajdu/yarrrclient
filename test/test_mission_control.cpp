#include "../src/mission_control.hpp"
#include "../src/local_event_dispatcher.hpp"
#include "../src/wakeup.hpp"
#include "test_services.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/mission_container.hpp>
#include <igloo/igloo_alt.h>
#include <thectci/service_registry.hpp>

using namespace igloo;

Describe( a_mission_control )
{
  void update_time()
  {
    the::ctci::service< LocalEventDispatcher >().wakeup.dispatch(
        yarrrc::EverySecond( 0u ) );
  }


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
    update_time();
    graphical_engine->draw_objects();
  }

  void SetUp()
  {
    graphical_engine = &test::get_cleaned_up_graphical_engine();
    mission_source.reset( new the::ctci::Dispatcher() );
    mission_control.reset( new yarrrc::MissionControl( *graphical_engine, *mission_source ) );

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
    the::ctci::service< LocalEventDispatcher >().wakeup.clear();
  }

  bool was_tokenized_text_printed( const std::string& text )
  {
    for ( const auto& token : yarrrc::tokenize( text, { 0, 0, 0, 0 } ) )
    {
      if ( !graphical_engine->was_printed( token.text() ) )
      {
        return false;
      }
    }
    return true;
  }

  void assert_tokenized_text_was_printed( const std::string& text )
  {
    AssertThat( was_tokenized_text_printed( text ), Equals( true ) );
  }

  It( prints_out_mission_name )
  {
    std::cout << "before mission update" << std::endl;
    update_mission( yarrr::ongoing );
    std::cout << "after mission update" << std::endl;
    assert_tokenized_text_was_printed( mission_name );
  }


  It( prints_out_mission_description )
  {
    update_mission( yarrr::ongoing );
    assert_tokenized_text_was_printed( mission_description );
  }

  It( prints_out_objectives )
  {
    update_mission( yarrr::ongoing );
    assert_tokenized_text_was_printed( objective_description );
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

  test::GraphicalEngine* graphical_engine;
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
    [ this ]( yarrr::Mission& ) -> yarrr::TaskState
    {
      thelog( yarrr::log::debug )( "Objective update called." );
      was_mission_updated = true;
      return objective_state;
    } };
  bool was_mission_finished_sent_out;
  yarrr::Mission::Id finished_mission_id;
};

