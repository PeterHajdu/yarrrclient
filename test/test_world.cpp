#include "../src/world.hpp"
#include <yarrr/object_container.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe(a_world)
{
  void SetUp()
  {
    objects.reset( new yarrr::ObjectContainer() );
    world.reset( new yarrrc::World( *objects ) );
  }

  It ( handles_focused_object_deletion )
  {
  }

  std::unique_ptr< yarrr::ObjectContainer > objects;
  std::unique_ptr< yarrrc::World > world;

};

