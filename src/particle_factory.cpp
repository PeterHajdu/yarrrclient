#include "particle_factory.hpp"
#include <thectci/service_registry.hpp>
#include <yarrr/graphical_engine.hpp>


ParticleFactory::ParticleFactory( yarrr::ParticleContainer& particles )
  : m_particles( particles )
{
}

void
ParticleFactory::create( const yarrr::PhysicalParameters& physical_parameters )
{
  m_particles.push_back( yarrr::Particle::Pointer( new yarrr::Particle(
        physical_parameters,
        the::ctci::service< yarrr::GraphicalEngine >() ) ) );
}

