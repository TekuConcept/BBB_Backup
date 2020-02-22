#
# Created by TekuConcept on February 21, 2020
#
# Example CMake file demonstrating how to use
# the kernel build tools. The example would
# generate a "foo.ko" kernel module.
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.8.2)
PROJECT(foo_module)

# SET(KMODULE_ARCH "arm")
# SET(KMODULE_CROSS_COMPILER_PREFIX "arm-linux-unknown-")
# SET(KMODULE_CFLAGS "-g")

INCLUDE(CMake/KernelBuildTools.cmake)
FILE(GLOB_RECURSE foo_sources "foo/source/*.c")
INCLUDE_DIRECTORIES("foo/include")
ADD_KMODULE(foo ${foo_sources})

TARGET_KMODULE_INCLUDE_DIRECTORIES(foo "common/include")
TARGET_KMODULE_CFLAGS(foo "-Wno-error")
