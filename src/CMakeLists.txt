set(LIB_SOURCE_FILES
  sdl_engine.cpp
  network_service.cpp
  keyboard_handler.cpp
  world.cpp
  fps.cpp
  particle_factory.cpp
  hud.cpp
  cli.cpp
  terminal.cpp
  mission_control.cpp
  information_window.cpp
  colorizer.cpp
  window.cpp
  wakeup.cpp
  text_token.cpp
  login_handler.cpp
  authentication_token.cpp
  )

set(CLIENT_SOURCE_FILES
  yarrrclient.cpp
  services.cpp
  )

add_library(yarrrclientlib STATIC ${LIB_SOURCE_FILES})

add_executable(yarrrclient ${CLIENT_SOURCE_FILES})

set(SHIPYARD_SOURCE_FILES
  yarrrshipyard.cpp
  services.cpp
  )

add_executable(yarrrshipyard ${SHIPYARD_SOURCE_FILES})

set(STATIC_C++ "-static-libstdc++ -static-libgcc")
set(LIB_YARRR "-Wl,--whole-archive -lyarrr -Wl,--no-whole-archive")
target_link_libraries(yarrrclient yarrrclientlib SDL2 SDL2_ttf pthread thectci thenet thetime ${LIB_YARRR} ${STATIC_C++} theconf themodel zmq thelog lua ${LIBS})
install(TARGETS yarrrclient DESTINATION bin)

target_link_libraries(yarrrshipyard yarrrclientlib SDL2 SDL2_ttf pthread thectci thenet thetime ${LIB_YARRR} ${STATIC_C++} theconf themodel thelog lua ${LIBS})
install(TARGETS yarrrshipyard DESTINATION bin)

file (STRINGS "../package_version" CPACK_PACKAGE_VERSION)
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Peter Ferenc Hajdu")
set(CPACK_DEBIAN_PACKAGE_NAME "yarrr")
set(CPACK_DEBIAN_PACKAGE_NAME "yarrr")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-ttf-2.0-0, libsdl2-2.0-0")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "The best space shooter in the galaxy.")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/postinst")
include(CPack)

