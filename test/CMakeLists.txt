include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.13.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

foreach(testcase version;geometry)
    add_executable(probe_test_${testcase} ${testcase}.cpp)
    target_link_libraries(probe_test_${testcase}
        PUBLIC
            probe::probe
            GTest::gtest_main
    )

    gtest_discover_tests(probe_test_${testcase})
endforeach()