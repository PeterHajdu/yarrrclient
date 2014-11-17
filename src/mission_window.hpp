#pragma once

#include <yarrr/graphical_engine.hpp>
#include <yarrr/mission_container.hpp>

namespace the
{
namespace ctci
{
class Dispatcher;
}
}

namespace yarrrc
{

class MissionWindow : public yarrr::GraphicalObject
{
  public:
    MissionWindow( yarrr::GraphicalEngine&, the::ctci::Dispatcher& mission_source );

  private:
    virtual void draw() const override;
    yarrr::MissionContainer m_missions;
};

}

