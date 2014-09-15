#include "../src/hud.cpp"
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/physical_parameters.hpp>
#include <yarrr/object.hpp>
#include <yarrr/object_creator.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_hud )
{

  void SetUp()
  {
    object = yarrr::create_ship();
    physical_parameters = &yarrr::component_of< yarrr::PhysicalBehavior >( *object ).physical_parameters;
    physical_parameters->orientation = 123;
    physical_parameters->coordinate = yarrr::Coordinate{ 12938, 912898234 };
    physical_parameters->velocity = yarrr::Coordinate{ 256, -512 };
    physical_parameters->angular_velocity = 423;
    hud.reset( new yarrrc::Hud( graphical_engine, *object ) );
    graphical_engine.printed_texts.clear();
    graphical_engine.draw_objects();
  }


  bool was_printed( const std::string& text ) const
  {
    return std::any_of( std::begin( graphical_engine.printed_texts ), std::end( graphical_engine.printed_texts ),
        [ this, text ]( const std::string& line )
        {
          return line.find( text ) != std::string::npos;
        } );
  }

  It( prints_out_the_inventory )
  {
    AssertThat( was_printed( "inventory" ), Equals( true ) );
    AssertThat( was_printed( "canon" ), Equals( true ) );
  }

  It( prints_out_the_coordinates_of_the_ship_in_metres )
  {
    AssertThat( was_printed( "coordinate" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->coordinate.x ) ) ),
        Equals( true ) );
    AssertThat( was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->coordinate.y ) ) ),
        Equals( true ) );
  }

  It( prints_out_the_velocity_of_the_ship_in_metres_per_second )
  {
    AssertThat( was_printed( "velocity" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->velocity.x ) ) ), Equals( true ) );
    AssertThat( was_printed( std::to_string( yarrr::huplons_to_metres( physical_parameters->velocity.y ) ) ), Equals( true ) );
  }

  It( prints_out_the_integrity_of_the_ship )
  {
    AssertThat( was_printed( "integrity" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( physical_parameters->integrity ) ), Equals( true ) );
  }

  It( prints_out_the_orientation_of_the_ship_in_degrees )
  {
    AssertThat( was_printed( "orientation" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( yarrr::hiplon_to_degrees( physical_parameters->orientation ) ) ), Equals( true ) );
  }

  It( prints_out_the_angular_velocity_of_the_ship )
  {
    AssertThat( was_printed( "angular velocity" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( physical_parameters->angular_velocity ) ), Equals( true ) );
  }

  test::GraphicalEngine graphical_engine;
  yarrr::PhysicalParameters* physical_parameters;
  std::unique_ptr< yarrrc::Hud > hud;

  yarrr::Object::Pointer object;
};

