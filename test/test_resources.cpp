#include "../src/resources.hpp"
#include <igloo/igloo_alt.h>
#include <fstream>

using namespace igloo;

//context dependent tests, I wasn't able to mock access()
Describe( a_resource_finder )
{
  void touch_file( const std::string& path )
  {
    std::ofstream touched_file( path );
    touched_file.close();
  }

  void SetUp()
  {
    touch_file( second_folder + file_only_in_second_folder );
    resource_finder.reset( new yarrr::ResourceFinder( path_list ) );
  }

  It( returns_full_path )
  {
    const std::string full_path( resource_finder->find( "cp" ) );
    AssertThat( full_path, Equals( "/bin/cp" ) );
  }

  It( returns_cstr_convertable_path )
  {
    AssertThat( resource_finder->find( "cp" ).c_str(), Equals( "/bin/cp" ) );
  }

  It( returns_path_of_the_first_match )
  {
    const std::string full_path( resource_finder->find( file_only_in_second_folder ) );
    AssertThat( full_path, Equals( second_folder + file_only_in_second_folder ) );
  }

  It( returns_empty_string_if_the_file_was_not_found )
  {
    const std::string full_path( resource_finder->find( "owijfoisdjfoij" ) );
    AssertThat( full_path, Equals( "" ) );
  }


  std::unique_ptr< yarrr::ResourceFinder > resource_finder;
  const std::string file_only_in_second_folder{ "appletree" };
  const std::string second_folder{ "/tmp/" };
  const yarrr::ResourceFinder::PathList path_list{{ "/bin/", second_folder, "/home/" }};
};

