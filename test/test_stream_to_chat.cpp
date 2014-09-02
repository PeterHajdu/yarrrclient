#include "../src/stream_to_chat.hpp"
#include <yarrr/chat_message.hpp>
#include <thectci/dispatcher.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;


Describe(a_stream_to_chat_translator)
{
  void SetUp()
  {
    test_dispatcher.reset( new the::ctci::Dispatcher() );
    test_dispatcher->register_listener< yarrr::ChatMessage >(
        [ this ]( const yarrr::ChatMessage& chat_message )
        {
          m_chat_messages.push_back( chat_message.message() );
          m_message_senders.push_back( chat_message.sender() );
        } );

    test_stream_to_chat.reset( new yarrrc::StreamToChat( sender ) );
    test_stream_to_chat->register_dispatcher( *test_dispatcher );

    m_chat_messages.clear();
    m_message_senders.clear();

    test_stream_to_chat->stream() << some_text << std::endl;
    test_stream_to_chat->stream() << some_more_text << std::endl;
    test_stream_to_chat->flush();
  }


  It( dispatches_messages_with_the_given_sender )
  {
    AssertThat( m_message_senders, Has().Exactly( 2 ).EqualTo( sender ) );
  }

  It( dispatches_lines_as_chat_messages )
  {
    AssertThat( m_chat_messages, Has().Exactly( 1 ).EqualTo( some_text ) );
    AssertThat( m_chat_messages, Has().Exactly( 1 ).EqualTo( some_more_text ) );
  }

  const std::string sender{ "system" };
  const std::string some_text{ "some text" };
  const std::string some_more_text{ "some more text" };

  std::vector< std::string > m_chat_messages;
  std::vector< std::string > m_message_senders;

  std::unique_ptr< yarrrc::StreamToChat > test_stream_to_chat;
  std::unique_ptr< the::ctci::Dispatcher > test_dispatcher;
};

