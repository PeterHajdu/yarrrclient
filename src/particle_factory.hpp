#pragma once

#include <yarrr/particle.hpp>

namespace yarrr
{

class PhysicalParameters;

}

class ParticleFactory : public yarrr::ParticleFactory
{
  public:
    virtual ~ParticleFactory() = default;
    virtual void create( const yarrr::PhysicalParameters& ) override;
};

