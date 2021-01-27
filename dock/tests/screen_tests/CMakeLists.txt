project (dock_screen_test)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

aux_source_directory(. DOCK_SCREEN_TEST_SRCS)

add_executable(dock_screen_test ${DOCK_SCREEN_TEST_SRCS})
target_link_libraries(dock_screen_test dock gtest gmock)

# ctest cannot pass these ui tests, don't know why
include(GoogleTest)
gtest_discover_tests(dock_screen_test PROPERTIES LABELS UiTest)

set_target_properties(dock_screen_test 
    PROPERTIES 
    CXX_STANDARD 17
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
