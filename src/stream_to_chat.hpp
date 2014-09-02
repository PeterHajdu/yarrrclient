#pragma once

#include <string>
#include <sstream>
#include <thectci/dispatcher.hpp>

namespace yarrrc
{

class StreamToChat : public the::ctci::Dispatcher
{
  public:
    StreamToChat( const std::string& sender );
    std::ostream& stream();
    void flush();

  private:
    const std::string m_sender;
    std::stringstream m_stream;
};

}

