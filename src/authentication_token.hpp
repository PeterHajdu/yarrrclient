#pragma once

#include <string>

namespace yarrrc
{

void
save_authentication_token( const std::string& username, const std::string& token );

std::string
load_authentication_token( const std::string& username );

}

