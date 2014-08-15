
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
install(TARGETS yarrrclient DESTINATION ${deploy_directory}/bin)

