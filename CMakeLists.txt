# enable x.y version format
cmake_policy(SET CMP0048 NEW)
project(Uno VERSION 0.1)

cmake_minimum_required (VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

enable_testing()

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
)
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_subdirectory(externals)
add_subdirectory(dock)
add_subdirectory(games)

add_executable(main main.cpp)
target_link_libraries(main dock)
target_link_libraries(main uno_core)

set_target_properties(main PROPERTIES CXX_STANDARD 17)