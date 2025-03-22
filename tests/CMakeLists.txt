include(CTest)
enable_testing()

macro(add_util_test filename)
    add_executable(${filename} ${filename}.cpp)
    target_compile_features(${filename} PRIVATE cxx_std_20)
    target_compile_options(${filename} PRIVATE -O2 -Wall -Wextra -Wpedantic -Wstrict-overflow=2 -Wdisabled-optimization -Wsign-conversion
            -Wsign-promo -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wdouble-promotion
            -Wuseless-cast -Wformat=2 -Wimplicit-fallthrough -Wpessimizing-move -Wredundant-move -Wnoexcept -Wctor-dtor-privacy -Wswitch-default
            -Wconversion)
    add_test(
            NAME ${filename}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${filename} --no-intro --no-path-filenames
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endmacro()

add_util_test(test_assertions)
add_util_test(test_class)
add_util_test(test_cmdline)
add_util_test(test_color)
add_util_test(test_log)
add_util_test(test_math)
add_util_test(test_string)
