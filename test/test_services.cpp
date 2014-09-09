#include <yarrr/graphical_engine.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <thectci/service_registry.hpp>

namespace
{

the::ctci::AutoServiceRegister< yarrr::GraphicalEngine, test::GraphicalEngine >
  auto_test_graphical_engine_register;

}

