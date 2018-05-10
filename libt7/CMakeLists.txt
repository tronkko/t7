#
# Test-Driven Development Framework 7 for C
#
# Copyright (C) 2018 Toni Ronkko
# This file is part of T7.  T7 may be freely distributed under the MIT
# license.  For more information, see https://github.com/tronkko/t7
#
cmake_minimum_required (VERSION 3.3)
project (libt7)
set (T7_MAJOR 1)
set (T7_MINOR 0)
set (T7_PATCHLEVEL 0)

# Add option to enable/disable threads.  By default, multi-threaded operation
# is enabled.  To compile single-threaded versions, add option
# -DT7_DISABLE_THREADS=1 to cmake command line.
option (T7_DISABLE_THREADS "Disable multi-threading" OFF)
if (NOT T7_DISABLE_THREADS)
    find_package (Threads REQUIRED)
    include (CheckIncludeFiles)
    CHECK_INCLUDE_FILES (sched.h HAVE_SCHED_H)
    CHECK_INCLUDE_FILES (pthread.h HAVE_PTHREAD_H)
endif (NOT T7_DISABLE_THREADS)
if (T7_DISABLE_THREADS)
    MESSAGE(STATUS "Support for multiple threads disabled")
endif (T7_DISABLE_THREADS)

# Allow the maximum number of threads to be set with the
# -DT7_MAX_THREADS=50 option
set (T7_MAX_THREADS 50 CACHE STRING "Maximum number of concurrent threads")
set_property (CACHE T7_MAX_THREADS PROPERTY STRINGS 10 25 50 100 200 500 1000)

# Allow the maximum number of exit handlers to be set with the
# -DT7_MAX_EXIT_HANDLERS=50 option
set (T7_MAX_EXIT_HANDLERS 50 CACHE STRING "Maximum number of exit handlers")
set_property (CACHE T7_MAX_EXIT_HANDLERS PROPERTY STRINGS 50 100 200 500 1000)

# Configure a header file to pass some of the CMake settings
# to the source code
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/t7/config.h.in"
    "${PROJECT_BINARY_DIR}/t7/config.h"
)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/t7/features.h.in"
    "${PROJECT_BINARY_DIR}/t7/features.h"
)

# Add the binary tree to the search path for include files
# so that we will find config.h
include_directories ("${PROJECT_BINARY_DIR}")

# Add files to library
add_library (t7
    src/terminate.c
    src/exit-handler.c
    src/critical-section.c
    src/fixture.c
    src/memory.c
    src/tls.c
    src/allocator.c
    src/static-allocator.c
    src/thread.c
    src/simulate-failure.c
    src/faulty-allocator.c
    src/charset.c
)

# Add dependency to threads library.  This allows executable programs to use
# t7 in single and multi-threaded modes without requiring them to specify
# the threads library.
if (NOT T7_DISABLE_THREADS)
target_link_libraries (t7 Threads::Threads)
endif (NOT T7_DISABLE_THREADS)

# Define include directories for compiling the library itself
target_include_directories (t7 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)

# Use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include (GNUInstallDirs)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install (TARGETS t7
    EXPORT t7
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install (DIRECTORY "include/t7"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/t7"
    FILES_MATCHING PATTERN "*.h")
install (FILES "${PROJECT_BINARY_DIR}/t7/features.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/t7")

# Create export files (for using t7 from other projects)
install(EXPORT t7 DESTINATION share/t7)
export(TARGETS t7 FILE t7.cmake)

# Shorthand for defining tests
include (CTest)
add_custom_target (check-t7
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C ${CMAKE_CFG_INTDIR}
)
function (t7_test TEST_NAME)
    add_executable (${TEST_NAME} EXCLUDE_FROM_ALL ${ARGN})
    target_link_libraries (${TEST_NAME} t7)
    add_test (${TEST_NAME} ${TEST_NAME})
    add_dependencies (check-t7 ${TEST_NAME})
endfunction (t7_test)

# Build test programs
t7_test (t-exit-handler tests/t-exit-handler.c)
t7_test (t-fixture tests/t-fixture.c)
t7_test (t-tls tests/t-tls.c)
t7_test (t-allocator tests/t-allocator.c)
t7_test (t-memory tests/t-memory.c)
t7_test (t-static-allocator tests/t-static-allocator.c)
t7_test (t-critical-section tests/t-critical-section.c)
t7_test (t-thread tests/t-thread.c)
t7_test (t-simulate-failure tests/t-simulate-failure.c)
t7_test (t-faulty-allocator tests/t-faulty-allocator.c)
t7_test (t-charset tests/t-charset.c)


