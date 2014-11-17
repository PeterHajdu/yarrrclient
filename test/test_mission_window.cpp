#include "../src/mission_window.hpp"
#include <yarrr/test_graphical_engine.hpp>
#include <yarrr/mission_container.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( a_mission_window )
{

  void SetUp()
  {
    graphical_engine.printed_texts.clear();
  }

  test::GraphicalEngine graphical_engine;
};

