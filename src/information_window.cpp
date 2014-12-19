#include "information_window.hpp"
#include "wakeup.hpp"
#include <ctime>
#include <array>

namespace
{

std::string epoch_to_time( unsigned long epoch )
{
  const time_t timet( epoch );
  struct tm* timeinfo( std::gmtime( &timet ) );
  std::array< char, 80 > buffer;
  strftime( &buffer.front(), 80, "%T", timeinfo );
  return buffer.data();
}

}

namespace yarrrc
{

InformationWindow::InformationWindow( yarrr::GraphicalEngine& engine )
  : m_window(
      {},
      engine,
      { 0, 120 },
      { 200, 15 } )
{
  yarrrc::call_every_second(
      [ this ]( const yarrrc::EverySecond& event )
      {
        update_time_to( event.now );
      } );
}

void
InformationWindow::update_time_to( const the::time::Time& now )
{
 const auto epoch( now / the::time::Clock::ticks_per_second );
 m_window.set_content( { { std::string( "universe time: " ) + epoch_to_time( epoch ) } } );
}

}

