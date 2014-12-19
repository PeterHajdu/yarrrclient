#pragma once

#include <thectci/dispatcher.hpp>
#include <thetime/clock.hpp>
#include <thetime/once_in.hpp>

namespace yarrrc
{

class EverySecond
{
  public:
    add_ctci( "yarrrc_every_second" );

    EverySecond( const the::time::Time& time )
      : now( time )
    {
    }

    const the::time::Time now;
};

class FastWakeup
{
  public:
    add_ctci( "yarrrc_fast_wakeup" );

    FastWakeup()
    {
    }
};

void call_every_second( std::function< void( const EverySecond& ) > );

template < typename Clock >
class WakeupSender
{
  public:
    WakeupSender( Clock& clock, the::ctci::Dispatcher& wakeup_dispatcher )
      : m_wakeup_dispatcher( wakeup_dispatcher )
      , m_every_second(
          clock, the::time::Clock::ticks_per_second,
          [ &wakeup_dispatcher = m_wakeup_dispatcher ]( const the::time::Time& timestamp )
          {
            wakeup_dispatcher.dispatch( yarrrc::EverySecond( timestamp ) );
          } )
      , m_fast(
          clock, the::time::Clock::ticks_per_second / 10,
          [ &wakeup_dispatcher = m_wakeup_dispatcher ]( const the::time::Time& timestamp )
          {
            wakeup_dispatcher.dispatch( yarrrc::FastWakeup() );
          } )
    {
    }

    void tick()
    {
      m_every_second.tick();
      m_fast.tick();
    }

  private:
    the::ctci::Dispatcher& m_wakeup_dispatcher;
    the::time::OnceIn< Clock > m_every_second;
    the::time::OnceIn< Clock > m_fast;
};


}

