#
# Test-Driven Development Framework 7 for C
#
# Copyright (C) 2018 Toni Ronkko
# This file is part of T7.  T7 may be freely distributed under the MIT
# license.  For more information, see https://github.com/tronkko/t7
#
cmake_minimum_required (VERSION 3.3)
project (t7)

# Compile in Debug mode by default
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug
        CACHE STRING
        "Type of build: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE
    )
endif (NOT CMAKE_BUILD_TYPE)

# Add extra debug flags for Gcc
MESSAGE(STATUS "Building ${CMAKE_BUILD_TYPE} version")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
if (CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "-W -Wall -Wextra -Wunreachable-code -Wswitch-default -Wswitch-enum -Wfloat-equal -Wdeclaration-after-statement -Wwrite-strings -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wsign-conversion -pedantic")
endif (CMAKE_COMPILER_IS_GNUCC)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

# Add distclean target
add_custom_target (distclean
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -P CMakeClean
)

# Add check target
add_custom_target (check
    COMMAND ${CMAKE_BUILD_TOOL} check-t7
)

# Create libraries
add_subdirectory (libt7)

# Create applications
add_subdirectory (inspire)


