# enable x.y version format
cmake_policy(SET CMP0048 NEW)
project(GameDock VERSION 0.9.3)

cmake_minimum_required (VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

add_subdirectory(externals)
add_subdirectory(dock)
add_subdirectory(games)

add_executable(sailgame main.cpp)
target_link_libraries(sailgame dock cxxopts)

set_target_properties(sailgame PROPERTIES CXX_STANDARD 17)