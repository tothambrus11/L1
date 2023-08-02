cmake_minimum_required(VERSION 3.25)
project(L1)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-fmodules-ts)
include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(L1
        main.cpp
        src/AST.h
        src/types.h
        tests/create_nodes.cpp
        tests/reductions.cpp
        src/reductions.h
)
target_link_libraries(L1 gtest_main gmock_main)
add_test(NAME example_test COMMAND L1)

