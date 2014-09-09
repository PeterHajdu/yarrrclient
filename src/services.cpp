#include <yarrr/engine_dispatcher.hpp>
#include <thectci/service_registry.hpp>

namespace
{

the::ctci::AutoServiceRegister< yarrr::EngineDispatcher, yarrr::EngineDispatcher >
  auto_engine_dispatcher_register;

}

