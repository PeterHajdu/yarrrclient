#include "../src/local_event_dispatcher.hpp"
#include <yarrr/graphical_engine.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <thectci/service_registry.hpp>

namespace
{

the::ctci::AutoServiceRegister< yarrr::GraphicalEngine, test::GraphicalEngine >
  auto_test_graphical_engine_register;

the::ctci::AutoServiceRegister< LocalEventDispatcher, LocalEventDispatcher >
  local_event_dispatcher_register;
}

