CMAKE_MINIMUM_REQUIRED(VERSION 3.2 FATAL_ERROR)

ADD_SUBDIRECTORY(depend)

PROJECT(Dusk VERSION 0.0.1)

### Global configuration

# Allow for custom organization of files in VisualStudio
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Allow for custom FindXXX.cmake files
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}/cmake")

### MSVC fixes

IF(MSVC)

    # Path to the VC directory
    SET(VC_ROOT $ENV{VS140COMNTOOLS}../../VC)
    STRING(REPLACE "\\" "/" VC_ROOT ${VC_ROOT})

    # Library and include search prefix for VisualStudio
    SET(VC_CMAKE_PREFIX ${VC_ROOT})

    # Force searching for 64-bit libraries first, if building 64-bit
    IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
        SET(VC_CMAKE_PREFIX ${VC_ROOT}/lib/amd64 ${VC_ROOT}/bin/amd64 ${VC_ROOT})
    ENDIF()

    # Prepend the cmake search prefix with our VisualStudio search path
    SET(CMAKE_PREFIX_PATH ${VC_CMAKE_PREFIX} ${CMAKE_PREFIX_PATH})

    # Prevent 'lib' from being prepended to libraries
    SET(CMAKE_SHARED_LIBRARY_PREFIX "")

ENDIF()

### Compiler-specific flags

# GCC or Clang
IF(CMAKE_COMPILER_IS_GNUCC OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")

    INCLUDE(CheckCXXCompilerFlag)

    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

    IF(COMPILER_SUPPORTS_CXX11)
        ADD_COMPILE_OPTIONS("-std=c++11")
    ELSEIF(COMPILER_SUPPORTS_CXX0X)
        ADD_COMPILE_OPTIONS("-std=c++0x")
    ELSE()
        MESSAGE(FATAL_ERROR "Compiler '${CMAKE_CXX_COMPILER}' has no C++11 support")
    ENDIF()

    ADD_DEFINITIONS(-D_XOPEN_SOURCE=700)
    ADD_COMPILE_OPTIONS(-Wall -Wextra -pedantic -Wno-unused-parameter
        -Wno-gnu-zero-variadic-macro-arguments -Wno-missing-braces)

# VisualStudio
ELSEIF(MSVC)

    ADD_DEFINITIONS(/D_CRT_SECURE_NO_WARNINGS)
    LIST(APPEND CMAKE_EXE_LINKER_FLAGS
        "/NODEFAULTLIB:MSVCRT"
        "/NODEFAULTLIB:LIBCMT")

ENDIF()

### Dusk library

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_BINARY_DIR}/include)

SET(Dusk_OUT Dusk)

SET(Dusk_CONFIG_IN include/dusk/config.hpp.in)
SET(Dusk_CONFIG    include/dusk/config.hpp)

SET(Dusk_INCLUDES
    include/dusk/dusk.hpp
)

SET(Dusk_SOURCES
    src/dusk.cpp
)

CONFIGURE_FILE(${Dusk_CONFIG_IN} ${Dusk_CONFIG})

ADD_LIBRARY(${Dusk_OUT}
    ${Dusk_CONFIG}
    ${Dusk_INCLUDES}
    ${Dusk_SOURCES}
)

SOURCE_GROUP("src" FILES ${Dusk_SOURCES})

### Example projects

ADD_SUBDIRECTORY(examples)
