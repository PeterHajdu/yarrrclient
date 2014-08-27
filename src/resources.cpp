#include "resources.hpp"
#include <unistd.h>
#include <string>
#include <array>

namespace
{

const std::array< std::string, 2 > resource_paths_in_priority_order =
{{
 "/usr/local/share/yarrr/",
 "/usr/share/yarrr/"
}};

}

namespace yarrr
{

std::string find_resource_file( const std::string& filename )
{
  for ( const auto& resource_prefix : resource_paths_in_priority_order )
  {
    const std::string full_path( resource_prefix + filename );
    if ( 0 == access( full_path.c_str(), R_OK ) )
    {
      return full_path;
    }
  }

  return "";
}

}

