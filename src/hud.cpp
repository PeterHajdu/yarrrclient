#include "hud.hpp"
#include "wakeup.hpp"
#include "local_event_dispatcher.hpp"
#include <yarrr/physical_parameters.hpp>
#include <yarrr/inventory.hpp>
#include <yarrr/cargo.hpp>
#include <yarrr/basic_behaviors.hpp>
#include <yarrr/object.hpp>
#include <yarrr/modell.hpp>
#include <thectci/service_registry.hpp>
#include <theui/list_restructure.hpp>

namespace
{
  const int hud_width( 250 );

  int calculate_x_from( const yarrr::Coordinate& screen_resolution )
  {
    return screen_resolution.x - hud_width;
  }

}

namespace yarrrc
{

Hud::Hud( yarrr::GraphicalEngine& graphical_engine, const yarrr::Object& object )
  : m_graphical_engine( graphical_engine )
  , m_physical_parameters( yarrr::component_of< yarrr::PhysicalBehavior >( object ).physical_parameters )
  , m_inventory( yarrr::component_of< yarrr::Inventory >( object ) )
  , m_cargo( yarrr::component_of< yarrr::CargoSpace >( object ) )
  , m_window(
      graphical_engine,
      { calculate_x_from( graphical_engine.screen_resolution() ), 120 },
      { hud_width, 500 },
      the::ui::front_from_top_with_fixed_height )
{
  the::ctci::Dispatcher& wakeup_dispatcher( the::ctci::service< LocalEventDispatcher >().wakeup );
  m_callback_token = wakeup_dispatcher.register_smart_listener< FastWakeup >(
      [ this ]( const FastWakeup& )
      {
        update_window();
      });
}


void
Hud::add_line( const TextToken& line )
{
  m_window.add_child( std::make_unique< TextBox >(
        TextToken::Container{ line },
        m_graphical_engine,
        the::ui::Window::Coordinate{ 0, 0 },
        the::ui::Size{ 0, line.height() } ) );
}


void
Hud::print_character()
{
  add_line( { "character", yarrr::Colour::Blue } );
  auto& characters( the::ctci::service< yarrr::ModellContainer >().get( "character" ) );
  if ( characters.empty() )
  {
    return;
  }

  auto& character( *std::begin( characters )->second );

  for ( auto& key_value : character )
  {
    const bool is_hidden{ '_' == key_value.first.front() };
    if ( is_hidden )
    {
      continue;
    }

    const auto value( key_value.second->get() );
    add_line( { key_value.first + ": " + value, yarrr::Colour::White } );
  }
}

void
Hud::update_window()
{
  m_window.clear();
  print_character();

  add_line( { "ship", yarrr::Colour::Blue } );
  add_line( { "integrity: " + std::to_string( m_physical_parameters.integrity ), yarrr::Colour::White } );
  add_line( { "coordinate: " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.coordinate.x ) ) + " , " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.coordinate.y ) ), yarrr::Colour::White } );

  add_line( { "velocity: " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.velocity.x ) ) + " , " +
      std::to_string( yarrr::huplons_to_metres( m_physical_parameters.velocity.y ) ), yarrr::Colour::White } );

  add_line( { "orientation: " +
      std::to_string( yarrr::hiplon_to_degrees( m_physical_parameters.orientation ) ), yarrr::Colour::White } );

  add_line( { "angular velocity: " +
      std::to_string( yarrr::hiplon_to_degrees( m_physical_parameters.angular_velocity ) ), yarrr::Colour::White } );

  add_line( { "inventory: ", yarrr::Colour::Blue } );
  for ( const auto& item : m_inventory.items() )
  {
    add_line( { " -> " + item.get().name(), yarrr::Colour::White } );
  }

  add_line( { "cargo: ", yarrr::Colour::Blue } );
  for ( const auto& goods : m_cargo.goods() )
  {
    add_line( { " -> " + goods.name, yarrr::Colour::White } );
  }
}

}

