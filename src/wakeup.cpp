#include "wakeup.hpp"
#include "local_event_dispatcher.hpp"

namespace yarrrc
{

void call_every_second( std::function< void( const EverySecond& ) > callback )
{
  the::ctci::Dispatcher& wakeup_dispatcher{ the::ctci::service< LocalEventDispatcher >().wakeup };
  wakeup_dispatcher.register_listener< yarrrc::EverySecond >( callback );
}

}

