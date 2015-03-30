#include "../src/terminal.hpp"
#include "test_services.hpp"
#include <yarrr/chat_message.hpp>
#include <yarrr/command.hpp>
#include <yarrr/protocol.hpp>
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

  It( prints_out_error_messages )
  {
    terminal->dispatch( error_message );
    graphical_engine->draw_objects();
    AssertThat( graphical_engine->last_printed_text, Contains( error_text ) );
  }

  It( prints_out_sender_of_chat_message )
  {
    AssertThat( graphical_engine->was_printed( chat_messages.back().sender() + ":" ), Equals( true ) );
  }

  const std::vector< yarrr::ChatMessage > chat_messages{
    { "a_test_message_1", "Kilgore_Trout_1" },
    { "a_test_message_2", "Kilgore_Trout_2" },
    { "a_test_message_3", "Kilgore_Trout_3" },
    { "a_test_message_4", "Kilgore_Trout_4" },
    { "a_test_message_5", "Kilgore_Trout_5" },
    { "a_test_message_6", "Kilgore_Trout_6" },
    { "a_test_message_7", "Kilgore_Trout_7" },
    { "a_test_message_8", "Kilgore_Trout_8" } };

  const std::string error_text{ "errortext" };
  const yarrr::Command error_message{ { yarrr::Protocol::error, error_text } };

  test::GraphicalEngine* graphical_engine;
  std::unique_ptr< yarrrc::Terminal > terminal;
};

