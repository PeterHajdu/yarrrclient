#pragma once

#include <yarrr/graphical_engine.hpp>
#include <thetime/frequency_meter.hpp>
#include <thetime/clock.hpp>

namespace the
{
namespace time
{

class Clock;

}
}

class FpsDrawer : yarrr::GraphicalObject
{
  public:
    FpsDrawer( the::time::Clock& clock );
    virtual ~FpsDrawer() = default;
    virtual void draw() const override;

  private:
    mutable the::time::FrequencyMeter< the::time::Clock > m_frequency_meter;
};

