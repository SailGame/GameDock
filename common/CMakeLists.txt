project(common)

aux_source_directory(. SOURCE)
add_subdirectory(archive)
add_subdirectory(component)
add_subdirectory(dock)
add_subdirectory(game)
add_subdirectory(grpc)
add_subdirectory(tests)

add_library(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} PUBLIC
  ftxui::screen
  ftxui::dom
  ftxui::component
  pb_lib
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
# to the library and the library users.
if (MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC "/utf-8")
endif()
