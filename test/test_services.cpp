#include "test_services.hpp"
#include "../src/local_event_dispatcher.hpp"
#include <yarrr/graphical_engine.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <thectci/service_registry.hpp>

namespace
{

the::ctci::AutoServiceRegister< LocalEventDispatcher, LocalEventDispatcher >
  local_event_dispatcher_register;

std::unique_ptr< test::GraphicalEngine > graphical_engine;

}


namespace test
{

test::GraphicalEngine&
get_cleaned_up_graphical_engine()
{
  graphical_engine = std::make_unique< test::GraphicalEngine >();
  the::ctci::ServiceRegistry::register_service< yarrr::GraphicalEngine >( *graphical_engine );
  return *graphical_engine;
}

}

