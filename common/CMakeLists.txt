project(common)

# After using add_subdirectory, we can now use the grpc targets directly from
# this build.
set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_REFLECTION grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
endif()

aux_source_directory(. SOURCE)
add_subdirectory(grpc)
add_subdirectory(home)
add_subdirectory(tests)

add_library(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} PRIVATE ftxui::component grpc++)

target_include_directories(${PROJECT_NAME} PUBLIC ..)

# C++17 is used. We requires fold expression at least.
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Force Microsoft Visual Studio to decode sources files in UTF-8. This applies
# to the library and the library users.
if (MSVC)
  target_compile_options(${PROJECT_NAME} PUBLIC "/utf-8")
endif()

# Add as many warning as possible:
if (WIN32)
  if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE "/W4")
    target_compile_options(${PROJECT_NAME} PRIVATE "/WX")
    target_compile_options(${PROJECT_NAME} PRIVATE "/wd4244")
    target_compile_options(${PROJECT_NAME} PRIVATE "/wd4267")
    target_compile_options(${PROJECT_NAME} PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
  endif()
  # Force Win32 to UNICODE
  target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE "-Wall")
  target_compile_options(${PROJECT_NAME} PRIVATE "-Wextra")
  target_compile_options(${PROJECT_NAME} PRIVATE "-pedantic")
  target_compile_options(${PROJECT_NAME} PRIVATE "-Werror")
  target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-sign-compare")
endif()