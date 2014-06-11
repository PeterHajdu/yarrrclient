
set(SOURCE_FILES
  main.cpp
    )

add_executable(yarrrclient ${SOURCE_FILES})
target_link_libraries(yarrrclient yarrr ${LIBS})
target_link_libraries(yarrrclient thenet ${LIBS})
install(TARGETS yarrrclient DESTINATION ${deploy_directory}/bin)

