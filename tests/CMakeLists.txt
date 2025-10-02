include(CTest)
enable_testing()

if (MSVC)
    set(COMPILE_OPTIONS /Od /Zi /W4 /Zc:preprocessor /D_CRT_NONSTDC_NO_WARNINGS)
    set(LINK_OPTIONS "")
else()
    set(COMPILE_OPTIONS -Og -ggdb3 -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wconversion -Wdeprecated
            -Wctor-dtor-privacy -Wdisabled-optimization -Wdouble-promotion -Wduplicated-branches -Wduplicated-cond
            -Wfloat-equal -Wformat=2 -Wformat-signedness -Wlogical-op -Wmissing-noreturn -Wnon-virtual-dtor
            -Wmissing-declarations -Wnull-dereference -Wnrvo -Wold-style-cast -Wredundant-decls -Wshadow
            -Wsign-conversion -Wsign-promo -Wstrict-aliasing=2 -Wstrict-overflow=2 -Wswitch-default -Wtype-limits
            -Wsuggest-attribute=returns_nonnull -Wundef -Wno-unknown-warning-option -Wuseless-cast -fstrict-aliasing)
endif()

if (GNU OR Clang)
    set(COMPILE_OPTIONS ${COMPILE_OPTIONS} -fsanitize=undefined,address,leak -fno-omit-frame-pointer)
    set(LINK_OPTIONS -fsanitize=undefined,address,leak -fno-omit-frame-pointer)
endif()

set(STB_STYLE
    process
)

macro(add_util_test filename)
    string(REPLACE "test_" "" util_name ${filename})

    set(SOURCES
        ${filename}.cpp
        external/ext/doctest_extensions.cpp
    )

    # if this util is STB‐style, add its impl stub
    list(FIND STB_STYLE ${util_name} _idx)
    if(_idx GREATER -1)
        list(APPEND SOURCES test_${util_name}_impl.cpp)
    endif()

    add_executable(${filename} ${SOURCES})
    target_include_directories(${filename} SYSTEM PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external
    )

    target_compile_options(${filename} PRIVATE ${COMPILE_OPTIONS})
    target_link_options(${filename} PRIVATE ${LINK_OPTIONS})
    add_test(
            NAME ${filename}
            COMMAND $<TARGET_FILE:${filename}> --no-intro --no-path-filenames
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endmacro()

add_util_test(test_class)
add_util_test(test_cli)
add_util_test(test_color)
add_util_test(test_common)
add_util_test(test_log)
add_util_test(test_math)
add_util_test(test_process)
add_util_test(test_string)
