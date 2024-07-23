cmake_minimum_required(VERSION 3.20)

if(EXISTS ${CMAKE_SOURCE_DIR}/user.cmake)
	include (user.cmake)
else()
    message(FATAL_ERROR "'user.cmake' does not exist!")
endif()

project(LynxMaker VERSION 0.1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# build runtime directory output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIGURATION>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>)

# additional cmake functions
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/install/lib/cmake)

set(g_projects
    gLog
    gFtp
    gCore
    gMqtt
    gMamba
    gArtnet
    gPromotion
    gSerializer
    gMulticastSocket
)

set(x_projects
	xMaker
	xDesignKit
)

set(as_subproject ${g_projects} ${x_projects})

# in source applications 
macro(find_package)
    if(NOT "${ARGV0}" IN_LIST as_subproject)
        message(STATUS "${ARGV0} not in source")
        _find_package(${ARGV})
    else()
        message(STATUS "${ARGV0} in source")
    endif()
endmacro()

if(MSVC)
    add_compile_options(/wd4828)
endif()

# install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)

# clean install directory
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/bin)
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/lib)
file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/include)

#define the requirement to compile (for example)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# apps
add_subdirectory(app/LynxMaker)

# gLib
add_subdirectory(gLib/gCore)
add_subdirectory(gLib/gLog)
add_subdirectory(gLib/gMqtt)
add_subdirectory(gLib/gArtnet)
add_subdirectory(gLib/gMamba)
add_subdirectory(gLib/gPromotion)
add_subdirectory(gLib/gFtp)
add_subdirectory(gLib/gSerializer)
add_subdirectory(gLib/gMulticastSocket)

add_subdirectory(xLib/xMaker)
add_subdirectory(xLib/xDesignKit)

install()
