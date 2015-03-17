#include "../src/terminal.hpp"
#include "test_services.hpp"
#include <yarrr/chat_message.hpp>
#include <thectci/dispatcher.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

Describe(a_terminal)
{
  void SetUp()
  {
    graphical_engine = &test::get_cleaned_up_graphical_engine();
    terminal.reset( new yarrrc::Terminal( *graphical_engine ) );
    graphical_engine->printed_texts.clear();

    for ( const auto& chat_message : chat_messages )
    {
      graphical_engine->printed_texts.clear();
      terminal->dispatch( chat_message );
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

  const std::vector< yarrr::ChatMessage > chat_messages{
    { "a test message 1", "Kilgore Trout 1" },
    { "a test message 2", "Kilgore Trout 2" },
    { "a test message 3", "Kilgore Trout 3" },
    { "a test message 4", "Kilgore Trout 4" },
    { "a test message 5", "Kilgore Trout 5" },
    { "a test message 6", "Kilgore Trout 6" },
    { "a test message 7", "Kilgore Trout 7" },
    { "a test message 8", "Kilgore Trout 8" } };

  test::GraphicalEngine* graphical_engine;
  std::unique_ptr< yarrrc::Terminal > terminal;
};

