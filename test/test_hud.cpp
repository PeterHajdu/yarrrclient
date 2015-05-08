#include "../src/hud.hpp"
#include "../src/local_event_dispatcher.hpp"
#include "../src/wakeup.hpp"
#include "test_services.hpp"
#include <yarrr/goods.hpp>
#include <yarrr/cargo.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/physical_parameters.hpp>
#include <yarrr/object.hpp>
#include <yarrr/object_creator.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_hud )
{
  void update()
  {
    the::ctci::service< LocalEventDispatcher >().wakeup.dispatch( yarrrc::FastWakeup() );
    graphical_engine->draw_objects();
  }

  void add_cargo_to( yarrr::Object& object )
  {
    auto& cargo( yarrr::component_of< yarrr::CargoSpace >( object ) );
    for ( const auto& g : goods )
    {
      cargo.add_goods( g );
    }
  }

  void SetUp()
  {
    graphical_engine = &test::get_cleaned_up_graphical_engine();
    object = test::create_ship();
    add_cargo_to( *object );
    physical_parameters = &yarrr::component_of< yarrr::PhysicalBehavior >( *object ).physical_parameters;
    physical_parameters->orientation = 123;
    physical_parameters->coordinate = yarrr::Coordinate{ 12938, 912898234 };
    physical_parameters->velocity = yarrr::Coordinate{ 256, -512 };
    physical_parameters->angular_velocity = 423;
    hud.reset( new yarrrc::Hud( *graphical_engine, *object ) );
    update();
  }

  It( prints_out_the_inventory )
  {
    AssertThat( graphical_engine->was_printed( "inventory" ), Equals( true ) );
    AssertThat( graphical_engine->was_printed( "canon" ), Equals( true ) );
  }

  It( prints_out_the_cargo )
  {
    AssertThat( graphical_engine->was_printed( "cargo" ), Equals( true ) );
    for ( const auto& g : goods )
    {
      AssertThat( graphical_engine->was_printed( g.name ), Equals( true ) );
    }
  }

  It( prints_out_the_coordinates_of_the_ship_in_metres )
  {
    AssertThat( graphical_engine->was_printed( "coordinate" ), Equals( true ) );
    AssertThat( graphical_engine->was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->coordinate.x ) ) ),
        Equals( true ) );
    AssertThat( graphical_engine->was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->coordinate.y ) ) ),
        Equals( true ) );
  }

  It( prints_out_the_velocity_of_the_ship_in_metres_per_second )
  {
    AssertThat( graphical_engine->was_printed( "velocity" ), Equals( true ) );
    AssertThat( graphical_engine->was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->velocity.x ) ) ), Equals( true ) );
    AssertThat( graphical_engine->was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->velocity.y ) ) ), Equals( true ) );
  }

  It( prints_out_the_integrity_of_the_ship )
  {
    AssertThat( graphical_engine->was_printed( "integrity" ), Equals( true ) );
    AssertThat( graphical_engine->was_printed( std::to_string( physical_parameters->integrity ) ), Equals( true ) );
  }

  It( prints_out_the_orientation_of_the_ship_in_degrees )
  {
    AssertThat( graphical_engine->was_printed( "orientation" ), Equals( true ) );
    AssertThat( graphical_engine->was_printed( std::to_string( yarrr::hiplon_to_degrees( physical_parameters->orientation ) ) ), Equals( true ) );
  }

  It( prints_out_the_angular_velocity_of_the_ship_in_meaningful_units )
  {
    AssertThat( graphical_engine->was_printed( "angular velocity" ), Equals( true ) );
    AssertThat( graphical_engine->was_printed( std::to_string( yarrr::hiplon_to_degrees( physical_parameters->angular_velocity ) ) ), Equals( true ) );
  }

  It( deregisters_callbacks_after_deleted )
  {
    hud.reset();
    update();
  }

  test::GraphicalEngine* graphical_engine;
  yarrr::PhysicalParameters* physical_parameters;
  std::unique_ptr< yarrrc::Hud > hud;

  const std::vector< yarrr::Goods > goods{ yarrr::Goods{ "a cargo", "", 10 }, yarrr::Goods{ "b cargo", "", 1 } };
  yarrr::Object::Pointer object;
};

