#include "particle_factory.hpp"
#include <thectci/service_registry.hpp>

namespace
{
  the::ctci::AutoServiceRegister< yarrr::ParticleFactory, ParticleFactory >
    auto_particle_factory_register;
}

void
ParticleFactory::create( const yarrr::PhysicalParameters& )
{
}

