#include "../src/text_token.hpp"
#include "test_services.hpp"
#include <yarrr/graphical_engine.hpp>
#include <yarrr/test_graphical_engine.hpp>
#include <thectci/service_registry.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace
{

std::string
concatenate_with_space_separator(
    const std::vector< std::string >& words )
{
  std::string concatenated_string;
  for ( const auto& word : words )
  {
    concatenated_string += word;
  }
  return concatenated_string;
}

}


Describe( text_tokens )
{
  void SetUp()
  {
    token = std::make_unique< yarrrc::TextToken >(
        initializing_text,
        initializing_colour );
  }

  It( sets_members_from_the_constructor )
  {
    AssertThat( token->text(), Equals( initializing_text ) );
    AssertThat( token->colour(), Equals( initializing_colour ) );
  }

  It( sets_the_default_colour_to_white )
  {
    const yarrrc::TextToken token_with_default_colour( "appletree" );
    AssertThat(
        token_with_default_colour.colour(),
        Equals( yarrr::Colour::White ) );
  }

  It( returns_the_height_of_the_token )
  {
    AssertThat( token->height(), IsGreaterThan( 0 ) );
  }

  It( asks_the_graphical_engine_to_calculate_token_width )
  {
    auto& test_graphics( test::get_cleaned_up_graphical_engine() );
    const yarrr::Size expected_rendered_text_size{ 123, 987 };
    test_graphics.text_size = expected_rendered_text_size;
    AssertThat( token->width(), Equals( expected_rendered_text_size.width ) );
  }

  const std::string initializing_text{ "token text" };
  const yarrr::Colour initializing_colour{ 0, 1, 2, 3 };
  std::unique_ptr< yarrrc::TextToken > token;
};


Describe(tokenizer)
{

  void SetUp()
  {
    tokens = yarrrc::tokenize( text, expected_colour );
  }

  It( splits_strings_to_words )
  {
    AssertThat( tokens, HasLength( words.size() ) );
    for ( const auto& word : words )
    {
      AssertThat( tokens, Contains( yarrrc::TextToken( word ) ) );
    }
  }

  It( tokenizes_the_last_word_even_if_there_are_no_spaces_at_the_end )
  {
    const std::string text_without_space( "dogfood" );
    const auto tokens( yarrrc::tokenize( text_without_space, expected_colour ) );
    AssertThat( tokens, !IsEmpty() );
    AssertThat( tokens.back().text(), Contains( text_without_space ) );
  }

  It( sets_the_token_colour )
  {
    AssertThat( tokens.back().colour(), Equals( expected_colour ) );
  }

  const std::vector< std::string > words{
    "this ", "is ", "a ", "simple ", "string " };
  const std::string text{ concatenate_with_space_separator( words ) };
  yarrrc::TextToken::Container tokens;
  const yarrr::Colour expected_colour{ 123, 12, 12, 3 };
};

