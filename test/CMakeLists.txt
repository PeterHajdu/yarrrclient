# required for igloo:
add_definitions(-Wno-deprecated-declarations)

set(TEST_SOURCE_FILES
    test_runner.cpp
    test_hud.cpp
    test_cli.cpp
    test_terminal.cpp
    test_services.cpp
    test_world.cpp
    test_mission_control.cpp
    test_information_window.cpp
    test_window.cpp
    test_wakeup.cpp
    test_text_token.cpp
    test_login_handler.cpp
    )


add_executable(test_runner EXCLUDE_FROM_ALL ${TEST_SOURCE_FILES})

set(STATIC_C++ "-static-libstdc++ -static-libgcc")
set(LIB_YARRR "-Wl,--whole-archive -lyarrr -Wl,--no-whole-archive")
target_link_libraries(test_runner yarrrclientlib ${LIB_YARRR} thelog thenet theconf themodel thectci thetime ${STATIC_C++} lua ${LIBS})

get_target_property(TEST_RUNNER_BIN test_runner LOCATION)

find_program( VALGRIND_PATH valgrind )

if(VALGRIND_PATH)
  set(RUNNER
    valgrind --leak-check=full --show-reachable=yes --log-file="valgrind.log"
    )
endif()

add_custom_target(test DEPENDS test_runner)
add_custom_command(TARGET test COMMAND ${RUNNER} ${TEST_RUNNER_BIN} --verbose DEPENDS test_runner)
