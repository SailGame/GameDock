project (uno_test)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

aux_source_directory(. UNO_TEST_SRCS)

add_executable(uno_test ${UNO_TEST_SRCS})
target_link_libraries(uno_test dock uno_core gtest gmock)

include(GoogleTest)
gtest_discover_tests(uno_test PROPERTIES LABELS UnoTest)

set_target_properties(uno_test 
    PROPERTIES 
    CXX_STANDARD 17
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
