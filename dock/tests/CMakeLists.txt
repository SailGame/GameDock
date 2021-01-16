add_executable(interactive_ui_test interactive_ui_test.cpp)
target_link_libraries(interactive_ui_test dock gtest gmock)

set_target_properties(interactive_ui_test 
    PROPERTIES 
    CXX_STANDARD 17
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_subdirectory(screen_tests)