
set(SOURCE_FILES
  main.cpp
  sdl_engine.cpp
  network_service.cpp
  keyboard_handler.cpp
  local_event_dispatcher.cpp
    )

add_executable(yarrrclient ${SOURCE_FILES})
target_link_libraries(yarrrclient SDL2 pthread thectci thenet thetime yarrr ${LIBS})
install(TARGETS yarrrclient DESTINATION ${deploy_directory}/bin)

