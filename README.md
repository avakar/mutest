# mutest

Ridiculously simple C++ unit test library with excellent CMake integration.

## Getting Started

Add mutest as a submodule to your project and include it via FetchContent.

    include(FetchContent)
    FetchContent_Declare(avakar.mutest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/mutest")
    FetchContent_MakeAvailable(avakar.mutest)

Using FetchContent ensures that when multiple projects depend on mutest,
only one instance -- the one declared first -- gets used.
It will also let the parent of your project decide which version to use.
Generally, dependencies should always be pulled via FetchContent.

If you're lazy, however, you can directly use `add_subdirectory`.

    add_subdirectory(deps/mutest)

Here, `mutest` will automatically integrate itself via FetchContent.
In this case, however, your subrepo must be checked out even if it ultimately
doesn't get used.

Once included, call `add_mutest` from your `CMakeLists.txt` to add both
source files and link dependencies.

    add_mutest(test/test.cpp my::library)

Only projects enumerated in `-DMUTEST_PROJECTS` will have their tests included.
Regular expressions are allowed;
you can set `-DMUTEST_PROJECTS=.*` to run tests for your project and all
its direct and indirect dependencies.
When unset, it defaults to your top-most project.

At the end of CMake configuration,
regardless of how many times you've called `add_mutest`,
a single new executable called `mutests` will be created.
If your tests are failing, this is the target to debug.

If you've enabled testing (or included CTest), a test case will
be registered that runs the `mutests` executable.
Otherwise, to make your testing simpler, a custom target named `RUN_MUTESTS`
will be added and included in ALL.
It will be run automatically if you build everything.

## Writing test cases

Use `mutest_case` macro to define a new unit test.
Add test assertions with `chk`.

    #include <avakar/mutest.h>
    mutest_case("it should honk")
    {
        goose untitled;
        untitled.press('Y');
        chk untitled.honked();
    }

The name of the test need not be unique.
