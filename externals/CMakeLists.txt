# add_subdirectory(grpc)
add_subdirectory(googletest)
add_subdirectory(spdlog)
add_subdirectory(cxxopts)
set(BUILD_UNO_LIB ON)
set(BUILD_TEXAS_LIB ON)
add_subdirectory(CommonLib)

include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG        v3.0.0
)
FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()