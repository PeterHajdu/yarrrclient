#include "stream_to_chat.hpp"
#include <yarrr/chat_message.hpp>

namespace yarrrc
{

StreamToChat::StreamToChat( const std::string& sender )
  : m_sender( sender )
{
}

std::ostream&
StreamToChat::stream()
{
  return m_stream;
}

void
StreamToChat::flush()
{
  std::string line;
  while ( std::getline( m_stream, line ) )
  {
    dispatch( yarrr::ChatMessage( line, m_sender ) );
  }
}

}

