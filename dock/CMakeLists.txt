project(dock)

aux_source_directory(. DOCK_SRCS)
add_subdirectory(component)
add_subdirectory(core)
add_subdirectory(screen)
add_subdirectory(tests)
add_subdirectory(util)

add_library(${PROJECT_NAME} ${DOCK_SRCS})

target_link_libraries(${PROJECT_NAME} PUBLIC
  spdlog::spdlog
  game_core
  texas
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
# to the library and the library users.
if (MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC "/utf-8")
endif()
