#####################################
# Project-wide settings
#####################################
cmake_minimum_required(VERSION 3.15)

project(syscall_intercept)

#####################################
# CMake module imports
#####################################
include(ExternalProject)

#####################################
# Import external project from git
#####################################

IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i386")
    MESSAGE(STATUS "Compiling syscall intercept for x86_64 targets")
    ExternalProject_Add(syscall_intercept
            GIT_REPOSITORY https://github.com/pmem/syscall_intercept.git
            GIT_TAG ca4b13531f883597c2f04a40e095f76f6c3a6d22
            PREFIX ${CMAKE_CURRENT_BINARY_DIR}
            CMAKE_ARGS
            -DBUILD_TESTS=OFF
            -DBUILD_EXAMPLES=OFF
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    )
ELSEIF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "riscv64")
    MESSAGE(STATUS "Compiling syscall intercept for riscV64 targets")
    ExternalProject_Add(syscall_intercept
            GIT_REPOSITORY https://github.com/alpha-unito/syscall_intercept.git
            GIT_TAG d1cb7a8d2030dba0bb23c083e5eda206f5752a2f
            PREFIX ${CMAKE_CURRENT_BINARY_DIR}
            CMAKE_ARGS
            -DBUILD_TESTS=OFF
            -DBUILD_EXAMPLES=OFF
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    )
ELSE ()
    MESSAGE(FATAL_ERROR "Architecture: ${CMAKE_SYSTEM_PROCESSOR} is not yet supported. Aborting compilation")
ENDIF ()

