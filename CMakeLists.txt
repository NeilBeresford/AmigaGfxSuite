# make sure the minimum cmake version is compatible with your compiler
cmake_minimum_required(VERSION 3.27.0)

# set a project name and version number
project(AmigaGfxSuite VERSION 0.0.1)

# set C Make to use the C++20 Language version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

include(ExternalProject)

find_package(Git REQUIRED)

ExternalProject_Add(
    doctest
    PREFIX ${CMAKE_BINARY_DIR}/doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

#-----------------------------------------------------------------------------
# add the subdirectories for the library and the converters etc
#-----------------------------------------------------------------------------

# supporting libraries
add_subdirectory(zlib)
add_subdirectory(libpng)

# libraries and applications
add_subdirectory(AmigaGfxLib)
add_subdirectory(AmigaSpriteCompress)
add_subdirectory(AmigaPaletteMerge)

# unit testing
add_subdirectory(TestAmigaGfxLIB)
add_subdirectory(TestAmigaSpriteCompress)
add_subdirectory(TestAmigaPaletteMerge)
