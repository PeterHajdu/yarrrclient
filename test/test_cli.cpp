#include "../src/cli.hpp"
#include <yarrr/chat_message.hpp>
#include <yarrr/command.hpp>
#include <thectci/dispatcher.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;


Describe(a_cli)
{
  void SetUp()
  {
    test_engine.reset( new test::GraphicalEngine() );
    test_cli.reset( new yarrrc::Cli( x, y, *test_engine ) );
    test_cli->append( some_more_text );
    test_engine->draw_objects();

    test_dispatcher.reset( new the::ctci::Dispatcher() );
    test_dispatcher->register_listener< yarrr::ChatMessage >(
        [ this ]( const yarrr::ChatMessage& chat_message )
        {
          last_chat_message_dispatched = chat_message.message();
        } );

    test_dispatcher->register_listener< yarrr::Command >(
        [ this ]( const yarrr::Command& command )
        {
          last_command_dispatched = command;
        } );

    test_cli->register_dispatcher( *test_dispatcher );
    last_chat_message_dispatched.clear();
    last_command_dispatched = yarrr::Command();
  }

  It( dispatches_command_if_message_starts_with_a_slash )
  {
    test_cli->finalize();
    test_cli->append( slash_some_more_text );
    test_cli->finalize();
    AssertThat( last_command_dispatched.command(), Equals( some ) );
  }

  It( draws_default_prompt )
  {
    AssertThat( test_engine->last_printed_text, Contains( "$" ) );
  }

  It( prints_the_current_state )
  {
    AssertThat( test_engine->last_printed_text, Contains( some_more_text ) );
  }

  It( clears_the_prompt_when_finalized )
  {
    test_cli->finalize();
    test_engine->draw_objects();
    AssertThat( test_engine->last_printed_text, Equals( "$ " ) );
  }

  It( dispatches_chat_message_when_finalized )
  {
    test_cli->finalize();
    AssertThat( last_chat_message_dispatched, Equals( some_more_text ) );
  }

  It( does_not_dispatch_empty_messages )
  {
    test_cli->finalize();
    test_cli->finalize();
    AssertThat( last_chat_message_dispatched, Equals( some_more_text ) );
  }

  It( prints_to_the_given_position )
  {
    test_cli->finalize();
    AssertThat( test_engine->x_of_printed_text, Equals( x ) );
    AssertThat( test_engine->y_of_printed_text, Equals( y ) );
  }

  It( can_drop_the_last_character )
  {
    test_cli->backspace();
    test_cli->finalize();

    const std::string expected_message(
        std::begin( some_more_text ),
        std::end( some_more_text ) - 1 );
    AssertThat( last_chat_message_dispatched, Equals( expected_message ) );
  }

  const int x{ 300 };
  const int y{ 300 };

  const std::string some{ "some" };
  const std::string more_text{ "more text" };
  const std::string some_more_text{ some + " " + more_text };
  const std::string slash_some_more_text{ std::string( "/" ) + some_more_text };
  std::unique_ptr< test::GraphicalEngine > test_engine;
  std::unique_ptr< yarrrc::Cli > test_cli;

  std::string last_chat_message_dispatched;
  std::unique_ptr< the::ctci::Dispatcher > test_dispatcher;

  yarrr::Command last_command_dispatched;
};

