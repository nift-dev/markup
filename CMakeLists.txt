cmake_minimum_required(VERSION 3.16)
project(markup VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMARK_SOURCES
    vendor/cmark/blocks.c vendor/cmark/buffer.c vendor/cmark/cmark.c
    vendor/cmark/cmark_ctype.c vendor/cmark/houdini_href_e.c
    vendor/cmark/houdini_html_e.c vendor/cmark/houdini_html_u.c
    vendor/cmark/html.c vendor/cmark/inlines.c vendor/cmark/iterator.c
    vendor/cmark/node.c vendor/cmark/references.c vendor/cmark/render.c
    vendor/cmark/scanners.c vendor/cmark/utf8.c)

add_library(markup_core STATIC src/Markup.cpp ${CMARK_SOURCES})
target_include_directories(markup_core PUBLIC include PRIVATE vendor/cmark)
if(MSVC)
    target_compile_options(markup_core PRIVATE /W4)
else()
    target_compile_options(markup_core PRIVATE -Wall -Wextra -pedantic)
endif()

add_executable(markup_cli cli/main.cpp)
set_target_properties(markup_cli PROPERTIES OUTPUT_NAME markup)
target_link_libraries(markup_cli PRIVATE markup_core)

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    foreach(test_name IN ITEMS markdown_smoke markdown_adversarial commonmark_regressions profile_matrix)
        string(REPLACE "markdown_" "markup-" target_name "${test_name}")
        add_executable(${target_name} tests/${test_name}.cpp)
        target_link_libraries(${target_name} PRIVATE markup_core)
        add_test(NAME ${test_name} COMMAND ${target_name})
    endforeach()
    add_executable(markup-fuzz-smoke tests/fuzz_smoke.cpp)
    target_link_libraries(markup-fuzz-smoke PRIVATE markup_core)
    add_test(NAME fuzz_smoke COMMAND markup-fuzz-smoke 10000)
    add_test(NAME commonmark_0_31_2
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/commonmark_runner.py
                --program $<TARGET_FILE:markup_cli>
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()
