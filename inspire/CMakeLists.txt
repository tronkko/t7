#
# Test-Driven Development Framework 7 for C
#
# Copyright (C) 2018 Toni Ronkko
# This file is part of T7.  T7 may be freely distributed under the MIT
# license.  For more information, see https://github.com/tronkko/t7
#
cmake_minimum_required (VERSION 3.3)
project (inspire)

# Build executable
add_executable (inspire inspire.c)
target_link_libraries (inspire t7)


