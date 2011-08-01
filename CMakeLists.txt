include(cmake/Macros/icecc.cmake) # this must be the first line!

#TODO: try this
#set(BINDING_NAME QtTracker)
#project(${BINDING_NAME} <- to lower)

project(qttracker)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Macros/ ${CMAKE_MODULE_PATH})

find_package(PythonLibs REQUIRED)
find_package(PythonInterpWithDebug REQUIRED)
find_package(Shiboken 1.0.1 REQUIRED)
find_package(PySide 1.0.0 REQUIRED)
find_package(Qt4 4.6.2 REQUIRED)

SET(SUPPORTED_QT_VERSION "4.6.0")
find_package(QtTracker 6.12.10 REQUIRED )

include(BindingMacros)

set(BINDING_NAME QtTracker)
set(BINDING_API_MAJOR_VERSION "0")
set(BINDING_API_MINOR_VERSION "1")
set(BINDING_API_MICRO_VERSION "0")
set(BINDING_API_VERSION "${BINDING_API_MAJOR_VERSION}.${BINDING_API_MINOR_VERSION}.${BINDING_API_MICRO_VERSION}")
set(BINDING_VERSION ${BINDING_API_VERSION})


if(CMAKE_HOST_UNIX)
    option(ENABLE_GCC_OPTIMIZATION "Enable specific GCC flags to optimization library size and performance. Only available on Release Mode" 0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fvisibility=hidden -Wno-strict-aliasing")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    if(ENABLE_GCC_OPTIMIZATION)
        set(CMAKE_BUILD_TYPE Release)
        set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -Os -Wl,-O1")
        if(NOT CMAKE_HOST_APPLE)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--hash-style=gnu")
        endif()
    endif()

    if(CMAKE_HOST_APPLE)
        if (NOT QT_INCLUDE_DIR)
            set(QT_INCLUDE_DIR "/Library/Frameworks")
         endif()
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}" CACHE PATH "The subdirectory relative to the install prefix where libraries will be installed (default is /lib${LIB_SUFFIX})" FORCE)

include(${QT_USE_FILE})

# uninstall target
uninstall_target()

# Binding archive
set_binding_archive("python-qttracker")

# Sets result to SITE_PACKAGE variable
find_python_site_package_dir()

# Detect if the python libs were compiled in debug mode
# Sets result to PY_DEBUG variable
python_compiled_for_debug()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT PYTHON_DEBUG_LIBRARIES)
        message(FATAL_ERROR "Python debug library not found. Try compile PySide with -DCMAKE_BUILD_TYPE=Release")
    endif()
    if(NOT PY_DEBUG)
        message(WARNING "Compiling PySide with debug enabled, but the python executable was not compiled with debug support.")
    else()
        add_definitions("-DPy_DEBUG")
    endif()
    set(PYSIDE_PYTHON_LIBRARIES ${PYTHON_DEBUG_LIBRARIES})
else()
    set(PYSIDE_PYTHON_LIBRARIES ${PYTHON_LIBRARIES})
endif()


# GeneratorRunner binary, extra flags, etc.
find_program(GENERATORRUNNER_BINARY generatorrunner REQUIRED)
if (NOT GENERATORRUNNER_BINARY)
    message(FATAL_ERROR "You need to specify GENERATOR variable (-DGENERATOR=value)")
endif()

message("GENERATOR: ${GENERATORRUNNER_BINARY}")
message("GENERATORRUNNER_BINARY: ${GENERATORRUNNER_BINARY}")

set(GENERATOR_EXTRA_FLAGS --generatorSet=shiboken
                          --enable-parent-ctor-heuristic
                          --enable-pyside-extensions
                          --enable-return-value-heuristic)

if(WIN32 OR DEFINED AVOID_PROTECTED_HACK)
    message(STATUS "${BINDING_NAME} will be generated avoiding the protected hack!")
    set(GENERATOR_EXTRA_FLAGS ${GENERATOR_EXTRA_FLAGS} --avoid-protected-hack)
    add_definitions(-DAVOID_PROTECTED_HACK)
else()
    message(STATUS "${BINDING_NAME} will be generated using the protected hack!")
endif()

# Binding source code, tests, etc
add_subdirectory(${BINDING_NAME})

