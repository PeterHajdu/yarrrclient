#include "../src/hud.cpp"
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/physical_parameters.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_hud )
{

  void SetUp()
  {
    hud.reset( new yarrrc::Hud( graphical_engine, physical_parameters ) );
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


  It( prints_out_the_coordinates_of_the_ship )
  {
    AssertThat( was_printed( "coordinate" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( physical_parameters.coordinate.x ) ), Equals( true ) );
    AssertThat( was_printed( std::to_string( physical_parameters.coordinate.y ) ), Equals( true ) );
  }

  It( prints_out_the_velocity_of_the_ship )
  {
    AssertThat( was_printed( "velocity" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( physical_parameters.velocity.x ) ), Equals( true ) );
    AssertThat( was_printed( std::to_string( physical_parameters.velocity.y ) ), Equals( true ) );
  }

  It( prints_out_the_integrity_of_the_ship )
  {
    AssertThat( was_printed( "integrity" ), Equals( true ) );
    AssertThat( was_printed( std::to_string( physical_parameters.integrity ) ), Equals( true ) );
  }

  test::GraphicalEngine graphical_engine;
  yarrr::PhysicalParameters physical_parameters;
  std::unique_ptr< yarrrc::Hud > hud;
};

