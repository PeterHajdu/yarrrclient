#include "resources.hpp"
#include <unistd.h>
#include <string>
#include <array>

namespace yarrr
{

ResourceFinder::ResourceFinder( const PathList& pathlist )
  : m_pathlist( pathlist )
{
}


std::string
ResourceFinder::find( const std::string& filename ) const
{
  for ( const std::string& resource_prefix : m_pathlist )
  {
    //without the c_str() call it left the trailing 0 in the concatenated string on freebsd
    const std::string full_path( resource_prefix.c_str() + filename );

    if ( 0 == access( full_path.c_str(), R_OK ) )
    {
      return full_path;
    }
  }

  return "";
}

}

