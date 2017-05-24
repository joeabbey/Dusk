CMAKE_MINIMUM_REQUIRED(VERSION 3.2 FATAL_ERROR)
CMAKE_POLICY(SET CMP0048 NEW)

PROJECT(Dusk VERSION 0.0.1)

### Global configuration

# Allow for custom organization of files in VisualStudio
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Allow for custom FindXXX.cmake files
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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
        SET(CMAKE_CXX_FLAGS "-std=c++11")
    ELSEIF(COMPILER_SUPPORTS_CXX0X)
        SET(CMAKE_CXX_FLAGS "-std=c++0x")
    ELSE()
        MESSAGE(FATAL_ERROR "Compiler '${CMAKE_CXX_COMPILER}' has no C++11 support")
    ENDIF()

    ADD_DEFINITIONS(-D_XOPEN_SOURCE=700)
    ADD_COMPILE_OPTIONS(-Wall -Wextra -pedantic -Wno-unused-parameter
        -Wno-gnu-zero-variadic-macro-arguments -Wno-missing-braces
        -Wno-gnu-statement-expression -Wno-keyword-macro)

# VisualStudio
ELSEIF(MSVC)

    ADD_DEFINITIONS(/D_CRT_SECURE_NO_WARNINGS)
    LIST(APPEND CMAKE_EXE_LINKER_FLAGS
        "/NODEFAULTLIB:MSVCRT"
        "/NODEFAULTLIB:LIBCMT")

ENDIF()

### Required packages

FIND_PACKAGE(OpenGL      REQUIRED)
FIND_PACKAGE(ZLIB        REQUIRED)
FIND_PACKAGE(FlatBuffers REQUIRED)
FIND_PACKAGE(Lua         REQUIRED)
FIND_PACKAGE(SDL2        REQUIRED COMPONENTS image ttf)

### Dusk library

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_BINARY_DIR}/include)

# Allow imgui to #include "imgui.h"
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/imgui)
# Disable imconfig.h
ADD_DEFINITIONS(-DIMGUI_DISABLE_INCLUDE_IMCONFIG_H)

SET(Dusk_OUT Dusk)

SET(Dusk_CONFIG_IN include/dusk/Config.hpp.in)
SET(Dusk_CONFIG    include/dusk/Config.hpp)

SET(Dusk_INCLUDES
    include/dusk/Dusk.hpp
    include/dusk/Platform.hpp
    include/dusk/Benchmark.hpp
    include/dusk/Log.hpp
    include/dusk/Util.hpp
    include/dusk/Timer.hpp
    include/dusk/UI.hpp
    include/dusk/App.hpp
    include/dusk/Shader.hpp
    include/dusk/Component.hpp
    include/dusk/Actor.hpp
    include/dusk/Scene.hpp
    include/dusk/Camera.hpp
    include/dusk/Mesh.hpp
    include/dusk/Material.hpp
    include/dusk/Texture.hpp

    # imgui
    include/imgui/imgui.h
    include/imgui/imgui_internal.h
    include/imgui/imgui_impl_sdl_gl3.h

    # glad
    include/glad/glad.h
    include/KHR/khrplatform.h
)

SET(Dusk_SOURCES

    src/dusk/Dusk.cpp
    src/dusk/Util.cpp
    src/dusk/UI.cpp
    src/dusk/App.cpp
    src/dusk/Shader.cpp
    src/dusk/Component.cpp
    src/dusk/Actor.cpp
    src/dusk/Scene.cpp
    src/dusk/Camera.cpp
    src/dusk/Mesh.cpp
    src/dusk/Material.cpp
    src/dusk/Texture.cpp

    # imgui
    src/imgui/imgui.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_impl_sdl_gl3.cpp

    # glad
    src/glad/glad.c
)

CONFIGURE_FILE(${Dusk_CONFIG_IN} ${Dusk_CONFIG})

ADD_LIBRARY(${Dusk_OUT}
    ${Dusk_CONFIG}
    ${Dusk_INCLUDES}
    ${Dusk_SOURCES}
)
ADD_DEPENDENCIES(${Dusk_OUT} glm nlohmann_json)

TARGET_LINK_LIBRARIES(
    ${Dusk_OUT}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${LUA_LIBRARIES}
)

INCLUDE_DIRECTORIES(
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIRS}
    ${GLMM_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}

    # TODO: Move to depend/CMakeListst.txt
    ${CMAKE_SOURCE_DIR}/depend/glm
    ${CMAKE_SOURCE_DIR}/depend/json/src
)

SOURCE_GROUP("src" FILES ${Dusk_SOURCES})

# Copy includes to build directory
ADD_CUSTOM_TARGET(copy-includes)
ADD_DEPENDENCIES(${Dusk_OUT} copy-includes)
FOREACH(inc ${Dusk_INCLUDES})
    ADD_CUSTOM_COMMAND(
        TARGET copy-includes PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
            copy ${CMAKE_SOURCE_DIR}/${inc} ${CMAKE_BINARY_DIR}/${inc}
    )
ENDFOREACH()

### Dependencies

ADD_SUBDIRECTORY(depend)

### Example projects

ADD_SUBDIRECTORY(examples)
