project (Tetris)

add_library(tetris
include/tetris/tetrisBox.hpp
src/tetrisBox.cpp
)
target_link_libraries(tetris PRIVATE ftxui::component)

foreach(lib tetris)
  target_include_directories(${lib}
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
      src
  )

  # C++17 is used. We requires fold expression at least.
  set_property(TARGET ${lib} PROPERTY CXX_STANDARD 17)

  # Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
  # to the library and the library users.
  if (MSVC)
    target_compile_options(${lib} PUBLIC "/utf-8")
  endif()

  # Add as many warning as possible:
  if (WIN32)
    if (MSVC)
      target_compile_options(${lib} PRIVATE "/W4")
      target_compile_options(${lib} PRIVATE "/WX")
      target_compile_options(${lib} PRIVATE "/wd4244")
      target_compile_options(${lib} PRIVATE "/wd4267")
      target_compile_options(${lib} PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
    endif()
    # Force Win32 to UNICODE
    target_compile_definitions(${lib} PRIVATE UNICODE _UNICODE)
  else()
    target_compile_options(${lib} PRIVATE "-Wall")
    target_compile_options(${lib} PRIVATE "-Wextra")
    target_compile_options(${lib} PRIVATE "-pedantic")
    target_compile_options(${lib} PRIVATE "-Werror")
    target_compile_options(${lib} PRIVATE "-Wno-sign-compare")
  endif()

endforeach()