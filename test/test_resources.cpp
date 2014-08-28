int access(const char *path, int mode)
{
  return 0;
}

#include "../src/resources.hpp"
#include <igloo/igloo_alt.h>

using namespace igloo;

//context dependent tests, I wasn't able to mock access()
Describe( a_resource_finder )
{
  void SetUp()
  {
    resource_finder.reset( new yarrr::ResourceFinder( path_list ) );
  }

  It( returns_full_path )
  {
    const std::string full_path( resource_finder->find( "cp" ) );
    AssertThat( full_path, Equals( "/bin/cp" ) );
  }

  It( returns_cstr_convertable_path )
  {
    std::cout << "test: " << resource_finder->find( "cp" ).c_str() << std::endl;
    AssertThat( resource_finder->find( "cp" ).c_str(), Equals( "/bin/cp" ) );
  }

  It( returns_path_of_the_first_match )
  {
    const std::string full_path( resource_finder->find( "diff" ) );
    AssertThat( full_path, Equals( "/usr/bin/diff" ) );
  }

  It( returns_empty_string_if_the_file_was_not_found )
  {
    const std::string full_path( resource_finder->find( "owijfoisdjfoij" ) );
    AssertThat( full_path, Equals( "" ) );
  }


  std::unique_ptr< yarrr::ResourceFinder > resource_finder;
  const yarrr::ResourceFinder::PathList path_list{{ "/bin/", "/usr/bin/", "/home/" }};
};

