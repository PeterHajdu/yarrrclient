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


  It( prints_out_the_coordinates_of_the_ship )
  {
    AssertThat( graphical_engine.printed_texts, Contains( std::to_string( physical_parameters.coordinate.x ) ) );
    AssertThat( graphical_engine.printed_texts, Contains( std::to_string( physical_parameters.coordinate.y ) ) );
  }

  It( prints_out_the_velocity_of_the_ship )
  {
    AssertThat( graphical_engine.printed_texts, Contains( std::to_string( physical_parameters.velocity.x ) ) );
    AssertThat( graphical_engine.printed_texts, Contains( std::to_string( physical_parameters.velocity.y ) ) );
  }

  It( prints_out_the_integrity_of_the_ship )
  {
    AssertThat( graphical_engine.printed_texts, Contains( std::to_string( physical_parameters.integrity ) ) );
  }

  test::GraphicalEngine graphical_engine;
  yarrr::PhysicalParameters physical_parameters;
  std::unique_ptr< yarrrc::Hud > hud;
};

