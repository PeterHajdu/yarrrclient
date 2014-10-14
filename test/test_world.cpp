#include "../src/world.hpp"
#include <yarrr/object_container.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe(a_world)
{
  It ( is_instantiable )
  {
    yarrr::ObjectContainer objects;
    yarrrc::World world( objects );
  }
};

