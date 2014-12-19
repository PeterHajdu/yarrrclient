#pragma once

#include <thectci/dispatcher.hpp>
#include <thectci/service_registry.hpp>

class LocalEventDispatcher
{
  public:
    add_ctci( "local_event_dispatcher" );
    the::ctci::Dispatcher dispatcher;
    the::ctci::Dispatcher incoming;
    the::ctci::Dispatcher outgoing;
    the::ctci::Dispatcher wakeup;
};

namespace yarrrc
{

template < typename T >
void local_dispatch( const T& event )
{
  the::ctci::service< LocalEventDispatcher >().dispatcher.dispatch( event );
}

}

