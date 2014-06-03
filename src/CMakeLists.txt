
set(SOURCE_FILES
  main.cpp
    )

add_executable(yarrrclient ${SOURCE_FILES})
target_link_libraries(yarrrclient yarrr ${LIBS})
install(TARGETS yarrrclient DESTINATION ${deploy_directory}/bin)

