project (dock_test)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

aux_source_directory(. DOCK_TEST_SRCS)

add_executable(dock_test ${DOCK_TEST_SRCS})
target_link_libraries(dock_test dock gtest gmock)

include(GoogleTest)
gtest_discover_tests(dock_test PROPERTIES LABELS GameDock)

set_target_properties(dock_test 
    PROPERTIES 
    CXX_STANDARD 17
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
