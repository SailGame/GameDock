project (common_unit_test)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

aux_source_directory(. SOURCE)

add_executable(common_unit_test main.cpp ${SOURCE})
target_link_libraries(common_unit_test PRIVATE common gtest gmock)

include(GoogleTest)
gtest_discover_tests(common_unit_test PROPERTIES LABELS GameDock)

set_target_properties(common_unit_test PROPERTIES CXX_STANDARD 17)