#include "../src/wakeup.hpp"
#include "../src/local_event_dispatcher.hpp"
#include <thectci/dispatcher.hpp>
#include <thetime/test_clock.hpp>
#include <igloo/igloo_alt.h>

using namespace igloo;

namespace test
{

using WakeupSender = yarrrc::WakeupSender< the::time::test::Clock >;

}

Describe(a_wakeup_sender)
{
  void SetUp()
  {
    was_every_second_dispatched = false;
    was_fast_wakeup_dispatched = false;
    dispatcher = std::make_unique< the::ctci::Dispatcher >();
    dispatcher->register_listener< yarrrc::EverySecond >(
        [
          &was_every_second_dispatched = was_every_second_dispatched,
          &last_woken_up_at = last_woken_up_at
        ]( const yarrrc::EverySecond& event )
        {
          last_woken_up_at = event.now;
          was_every_second_dispatched = true;
        } );

    dispatcher->register_listener< yarrrc::FastWakeup >(
        [
          &was_fast_wakeup_dispatched = was_fast_wakeup_dispatched
        ]( const yarrrc::FastWakeup& event )
        {
          was_fast_wakeup_dispatched = true;
        } );

    wakeup_sender = std::make_unique< test::WakeupSender >( clock, *dispatcher );
    clock.sleep_for( the::time::Clock::ticks_per_second );
    wakeup_sender->tick();
  }

  It( dispatches_fast_wakeup_events )
  {
    AssertThat( was_fast_wakeup_dispatched, Equals( true ) );
  }

  It( dispatches_every_second_events )
  {
    AssertThat( was_every_second_dispatched, Equals( true ) );
  }

  It( sends_current_time_with_the_event )
  {
    AssertThat( last_woken_up_at, Equals( clock.now() ) );
  }

  bool was_every_second_dispatched;
  bool was_fast_wakeup_dispatched;
  the::time::Time last_woken_up_at;
  the::time::test::Clock clock;
  std::unique_ptr< the::ctci::Dispatcher > dispatcher;
  std::unique_ptr< test::WakeupSender > wakeup_sender;
};


Describe( call_every_second )
{
  void TearDown()
  {
    the::ctci::service< LocalEventDispatcher >().wakeup.clear();
  }

  It( registers_the_callback_function_to_the_wakeup_dispatcher )
  {
    bool was_callback_called{ false };
    yarrrc::call_every_second(
        [ &was_callback_called ]( const yarrrc::EverySecond& )
        {
          was_callback_called = true;
        } );

    the::ctci::service< LocalEventDispatcher >().wakeup.dispatch( yarrrc::EverySecond( 0u ) );
    AssertThat( was_callback_called, Equals( true ) );
  }

};

