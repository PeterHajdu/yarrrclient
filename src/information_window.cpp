#include "information_window.hpp"
#include <thetime/clock.hpp>
#include <iostream>
#include <ctime>
#include <array>

namespace
{

std::string epoch_to_time( unsigned long epoch )
{
  const time_t timet( epoch );
  struct tm* timeinfo( std::localtime( &timet ) );
  std::array< char, 80 > buffer;
  strftime( &buffer.front(), 80, "%T", timeinfo );
  return buffer.data();
}

}

namespace yarrrc
{

InformationWindow::InformationWindow( yarrr::GraphicalEngine& engine, const the::time::Clock& clock )
  : m_window( 0, 120, engine, [ this ](){ return generate_lines(); } )
  , m_clock( clock )
{
}

ListWindow::Lines
InformationWindow::generate_lines() const
{
  ListWindow::Lines lines;
  lines.push_back( { "universe time: ", yarrr::Colour::White } );
  const auto epoch( m_clock.now() / the::time::Clock::ticks_per_second );
  lines.push_back( { epoch_to_time( epoch ), yarrr::Colour::White } );
  return lines;
}

}

