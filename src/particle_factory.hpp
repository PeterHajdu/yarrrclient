#pragma once

#include <yarrr/particle.hpp>
#include <vector>

namespace yarrr
{

class PhysicalParameters;

}

class ParticleFactory : public yarrr::ParticleFactory
{
  public:
    virtual ~ParticleFactory() = default;
    ParticleFactory( yarrr::ParticleContainer& );
    virtual void create( const yarrr::PhysicalParameters& ) override;

  private:
    yarrr::ParticleContainer& m_particles;
};

