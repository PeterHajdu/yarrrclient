#include "../src/terminal.hpp"
#include "test_services.hpp"
#include <yarrr/chat_message.hpp>
#include <thectci/dispatcher.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace
{
  std::string formatted_chat_message( const yarrr::ChatMessage& chat_message )
  {
    return chat_message.message();
  }
}

Describe(a_terminal)
{
  void SetUp()
  {
    graphical_engine = &test::get_cleaned_up_graphical_engine();
    test_terminal.reset( new yarrrc::Terminal( *graphical_engine, number_of_terminal_lines ) );
    graphical_engine->printed_texts.clear();

    for ( const auto& chat_message : chat_messages )
    {
      graphical_engine->printed_texts.clear();
      test_terminal->dispatch( chat_message );
      graphical_engine->draw_objects();
    }

  }

  It( prints_out_chat_messages )
  {
    AssertThat( graphical_engine->last_printed_text, Contains( chat_messages.back().message() ) );
  }

  It( prints_out_sender_of_chat_message )
  {
    AssertThat( graphical_engine->was_printed( chat_messages.back().sender() + ":" ), Equals( true ) );
  }

  void assert_drew_messages_between( size_t begin, size_t end )
  {
    if ( begin > 0 )
    {
      AssertThat( graphical_engine->printed_texts, !Contains( formatted_chat_message( chat_messages[ begin - 1 ] ) ) );
    }

    for ( size_t i{ begin }; i < end; ++i )
    {
      AssertThat( graphical_engine->printed_texts, Has().Exactly( 1 ).EqualTo( formatted_chat_message( chat_messages[ i ] ) ) );
    }

    if ( end < chat_messages.size() - 1 )
    {
      AssertThat( graphical_engine->printed_texts, !Contains( formatted_chat_message( chat_messages[ end + 1 ] ) ) );
    }
  }

  It( prints_out_only_the_last_n_chat_message )
  {
    const size_t first_on_screen{ number_of_messages - number_of_terminal_lines };
    assert_drew_messages_between( first_on_screen, number_of_messages );
  }

  It( can_be_scrolled_up )
  {
    test_terminal->scroll_up();
    graphical_engine->printed_texts.clear();
    graphical_engine->draw_objects();
    const size_t first_on_screen{ number_of_messages - number_of_terminal_lines - number_of_lines_scrolled };
    assert_drew_messages_between( first_on_screen, number_of_messages - number_of_lines_scrolled );
  }

  It( jumps_to_the_beginning_when_home_is_called )
  {
    test_terminal->home();
    graphical_engine->printed_texts.clear();
    graphical_engine->draw_objects();
    assert_drew_messages_between( 0, number_of_terminal_lines );
  }

  It( jumps_to_the_end_when_end_is_called )
  {
    test_terminal->home();
    test_terminal->end();
    graphical_engine->printed_texts.clear();
    graphical_engine->draw_objects();
    const size_t first_on_screen{ number_of_messages - number_of_terminal_lines };
    assert_drew_messages_between( first_on_screen, number_of_messages );
  }

  It( can_be_scrolled_up_only_till_the_first_message )
  {
    test_terminal->home();
    test_terminal->scroll_up();
    graphical_engine->printed_texts.clear();
    graphical_engine->draw_objects();
    assert_drew_messages_between( 0, number_of_terminal_lines );
  }

  It( can_be_scrolled_down )
  {
    test_terminal->scroll_up();
    test_terminal->scroll_down();
    graphical_engine->printed_texts.clear();
    graphical_engine->draw_objects();

    const size_t first_on_screen{ number_of_messages - number_of_terminal_lines };
    assert_drew_messages_between( first_on_screen, number_of_messages );
  }

  It( can_be_scrolled_down_only_till_the_last_page )
  {
    test_terminal->scroll_down();
    graphical_engine->printed_texts.clear();
    graphical_engine->draw_objects();

    const size_t first_on_screen{ number_of_messages - number_of_terminal_lines };
    assert_drew_messages_between( first_on_screen, number_of_messages );
  }


  const size_t number_of_terminal_lines{ 4 };
  const size_t number_of_lines_scrolled{ number_of_terminal_lines / 2 };
  const std::vector< yarrr::ChatMessage > chat_messages{
    { "a test message 1", "Kilgore Trout 1" },
    { "a test message 2", "Kilgore Trout 2" },
    { "a test message 3", "Kilgore Trout 3" },
    { "a test message 4", "Kilgore Trout 4" },
    { "a test message 5", "Kilgore Trout 5" },
    { "a test message 6", "Kilgore Trout 6" },
    { "a test message 7", "Kilgore Trout 7" },
    { "a test message 8", "Kilgore Trout 8" } };

  const size_t number_of_messages{ chat_messages.size() };

  test::GraphicalEngine* graphical_engine;
  std::unique_ptr< yarrrc::Terminal > test_terminal;
};

