
set(SOURCE_FILES
  main.cpp
  sdl_engine.cpp
  network_service.cpp
  keyboard_handler.cpp
  local_event_dispatcher.cpp
  world.cpp
  fps.cpp
  particle_factory.cpp
    )


add_executable(yarrrclient ${SOURCE_FILES})

set(LIB_YARRR "-Wl,--whole-archive -lyarrr -Wl,--no-whole-archive")
target_link_libraries(yarrrclient SDL2 SDL2_ttf pthread thelog theconf thectci thenet thetime ${LIB_YARRR} ${LIBS})
install(TARGETS yarrrclient DESTINATION bin)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Peter Ferenc Hajdu")
set(CPACK_DEBIAN_PACKAGE_NAME "yarrr")
set(CPACK_DEBIAN_PACKAGE_NAME "yarrr")
set(CPACK_DEBIAN_PACKAGE_VERSION 0.0.1)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "sdl2, sdl2_ttf")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "The best space shooter in the galaxy.")
set(CPACK_DEBIAN_PACKAGE_SECTION "games")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
include(CPack)

