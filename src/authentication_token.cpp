#include "authentication_token.hpp"
#include <theconf/configuration.hpp>

#include <fstream>

namespace
{

std::string
token_file_for_user( const std::string& username )
{
  return the::conf::get_value( "home_folder" ) + "/" + username + ".token";
}

}

namespace yarrrc
{

void save_authentication_token(
    const std::string& username,
    const std::string& token )
{
  std::ofstream( token_file_for_user( username ) ) << token;
}

std::string
load_authentication_token( const std::string& username )
{
  std::string token;
  std::ifstream token_file( token_file_for_user( username ) );
  token_file >> token;
  return token;
}

}

