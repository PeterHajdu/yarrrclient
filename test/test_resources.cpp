int access(const char *path, int mode)
{
  return 0;
}

#include "../src/resources.hpp"
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe( resource_finder )
{

  //todo: context dependent test, the files are looked up on the running machine
  It( returns_full_path_for_a_resource_file )
  {
    const std::string full_path( yarrr::find_resource_file( file_name ) );
    AssertThat( full_path, EndsWith( file_name ) );
    AssertThat( full_path,
        StartsWith( common_resource_path_variant_1 ) ||
        StartsWith( common_resource_path_variant_2 ) );
  }

  It( returns_empty_string_if_the_file_was_not_found )
  {
    const std::string full_path( yarrr::find_resource_file( name_of_not_existing_file ) );
    AssertThat( full_path, Equals( "" ) );
  }

  const std::string common_resource_path_variant_1{ "/usr/local/share/yarrr/" };
  const std::string common_resource_path_variant_2{ "/usr/share/yarrr/" };
  const std::string file_name{ "stuff.ttf" };
  const std::string name_of_not_existing_file{ "ablsdfjowiejfoweijfstuff.ttf" };
};

