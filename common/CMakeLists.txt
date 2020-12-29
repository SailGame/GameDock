project(common)

FIND_PACKAGE(Boost 1.69 COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

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

set(proto_dir "${CMAKE_CURRENT_SOURCE_DIR}/../externals/proto")
# Proto file
set(proto_files 
    "${CMAKE_CURRENT_SOURCE_DIR}/../externals/proto/core/core.proto"
    "${CMAKE_CURRENT_SOURCE_DIR}/../externals/proto/core/error.proto"
    "${CMAKE_CURRENT_SOURCE_DIR}/../externals/proto/core/provider.proto"
    "${CMAKE_CURRENT_SOURCE_DIR}/../externals/proto/core/types.proto"
)

# Generated sources
list(APPEND proto_srcs 
    "${CMAKE_CURRENT_BINARY_DIR}/core/core.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/core/error.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/core/provider.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/core/types.pb.cc"
)
list(APPEND proto_hdrs 
    "${CMAKE_CURRENT_BINARY_DIR}/core/core.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/core/error.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/core/provider.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/core/types.pb.h"
)
list(APPEND grpc_srcs 
    "${CMAKE_CURRENT_BINARY_DIR}/core/core.grpc.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/core/error.grpc.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/core/provider.grpc.pb.cc"
    "${CMAKE_CURRENT_BINARY_DIR}/core/types.grpc.pb.cc"
)
list(APPEND grpc_hdrs 
    "${CMAKE_CURRENT_BINARY_DIR}/core/core.grpc.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/core/error.grpc.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/core/provider.grpc.pb.h"
    "${CMAKE_CURRENT_BINARY_DIR}/core/types.grpc.pb.h"
)

add_custom_command(
    OUTPUT ${proto_srcs} ${proto_hdrs} ${grpc_srcs} ${grpc_hdrs}
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out=generate_mock_code=true:"${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${proto_dir}" -I "/usr/include/"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    ${proto_files}
    DEPENDS "${proto_files}" "${_PROTOBUF_PROTOC}" "${_GRPC_CPP_PLUGIN_EXECUTABLE}"
)

set_source_files_properties(${proto_srcs} ${grpc_srcs} PROPERTIES GENERATED TRUE)

aux_source_directory(. SOURCE)
add_subdirectory(archive)
add_subdirectory(component)
add_subdirectory(dock)
add_subdirectory(game)
add_subdirectory(grpc)
add_subdirectory(tests)

add_library(${PROJECT_NAME} ${SOURCE} ${proto_srcs} ${grpc_srcs})

target_link_libraries(${PROJECT_NAME}
  grpc++
  ftxui::screen
  ftxui::dom
  ftxui::component
  ${Boost_LIBRARIES}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${_REFLECTION}
)

target_include_directories(${PROJECT_NAME} PUBLIC .. PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

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
  target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-sign-compare")
endif()