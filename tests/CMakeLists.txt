include(CTest)
enable_testing()

if (MSVC)
    set(COMPILE_OPTIONS /Od /Zi /std:c++20 /W4 /Zc:preprocessor /D_CRT_NONSTDC_NO_WARNINGS)
    set(LINK_OPTIONS "")
else()
    set(COMPILE_OPTIONS -O0 -ggdb3 -std=c++20 -Wall -Wextra -Wpedantic -Wstrict-overflow=2 -Wdisabled-optimization -Wsign-conversion -Wsign-promo
            -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wdouble-promotion
            -Wuseless-cast -Wformat=2 -Wimplicit-fallthrough -Wpessimizing-move -Wredundant-move -Wnoexcept -Wctor-dtor-privacy -Wswitch-default
            -Wconversion -fsanitize=undefined,address,leak -fno-omit-frame-pointer)
    set(LINK_OPTIONS -fsanitize=undefined,address,leak -fno-omit-frame-pointer)
endif()

macro(add_util_test filename)
    add_executable(${filename} ${filename}.cpp external/ext/doctest_extensions.cpp)
    target_compile_features(${filename} PRIVATE cxx_std_20)
    target_compile_options(${filename} PRIVATE ${COMPILE_OPTIONS})
    target_link_options(${filename} PRIVATE ${LINK_OPTIONS})
    add_test(
            NAME ${filename}
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${filename} --no-intro --no-path-filenames
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endmacro()

add_util_test(test_class)
add_util_test(test_cmdline)
add_util_test(test_color)
add_util_test(test_common)
add_util_test(test_log)
add_util_test(test_math)
add_util_test(test_process)
add_util_test(test_string)
