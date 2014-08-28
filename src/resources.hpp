#pragma once

#include <thectci/id.hpp>

#include <string>
#include <vector>

namespace yarrr
{

class ResourceFinder
{
  public:
    add_ctci( "yarrr_resource_finder" );

    typedef std::vector< std::string > PathList;
    ResourceFinder( const PathList& );

    std::string find( const std::string& ) const;

  private:
    const PathList m_pathlist;
};

}

