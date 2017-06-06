CMAKE_MINIMUM_REQUIRED(VERSION 3.2 FATAL_ERROR)
CMAKE_POLICY(SET CMP0048 NEW)

PROJECT(Dusk VERSION 0.0.2)

# Add dependencies to be built with the project
ADD_SUBDIRECTORY(depend)

### Global configuration

# Allow for custom organization of files in VisualStudio
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Allow for custom FindXXX.cmake files
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Get Git revision
INCLUDE(GetGitRevisionDescription)
GET_GIT_HEAD_REVISION(GIT_REFSPEC DUSK_REVISION)
STRING(SUBSTRING "${DUSK_REVISION}" 0 12 DUSK_REVISION)

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
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "/EHsc")
    LIST(APPEND CMAKE_EXE_LINKER_FLAGS
        "/NODEFAULTLIB:MSVCRT"
        "/NODEFAULTLIB:LIBCMT")

ENDIF()

### Required packages

FIND_PACKAGE(OpenGL      REQUIRED)

INCLUDE_DIRECTORIES(BEFORE SYSTEM
    ${OPENGL_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    ${FLATBUFFERS_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${JSON_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
)

SET(Dusk_LINK_LIBRARIES
    ${OPENGL_LIBRARIES}
    ${LUA_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${FLATBUFFERS_LIBRARIES}
)
SET(Dusk_LINK_LIBRARIES_DEBUG
    ${LUA_LIBRARIES_DEBUG}
    ${ZLIB_LIBRARIES_DEBUG}
    ${GLFW_LIBRARIES_DEBUG}
    ${FLATBUFFERS_LIBRARIES_DEBUG}
)
SET(Dusk_LINK_LIBRARIES_RELEASE
    ${LUA_LIBRARIES_RELEASE}
    ${ZLIB_LIBRARIES_RELEASE}
    ${GLFW_LIBRARIES_RELEASE}
    ${FLATBUFFERS_LIBRARIES_RELEASE}
)

### Dusk library

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_BINARY_DIR}/include)

# Allow imgui to #include "imgui.h"
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/imgui)
# Disable imconfig.h
ADD_DEFINITIONS(-DIMGUI_DISABLE_INCLUDE_IMCONFIG_H)

SET(Dusk_OUT Dusk)

SET(Dusk_CONFIG_IN include/dusk/Config.hpp.in)
SET(Dusk_CONFIG    ${CMAKE_BINARY_DIR}/include/dusk/Config.hpp)

FILE(GLOB_RECURSE Dusk_ASSETS RELATIVE ${CMAKE_SOURCE_DIR} "assets/*")

SET(Dusk_INCLUDES
    include/dusk/Dusk.hpp
    include/dusk/Log.hpp
    include/dusk/Platform.hpp
    include/dusk/Benchmark.hpp
    include/dusk/Util.hpp
    include/dusk/ScriptHost.hpp
    include/dusk/Timer.hpp
    include/dusk/Event.hpp
    include/dusk/EventCallbacks.hpp
    include/dusk/EventDispatcher.hpp
    include/dusk/App.hpp
    include/dusk/UI.hpp
    include/dusk/Scene.hpp
    include/dusk/Camera.hpp
    include/dusk/Actor.hpp
    include/dusk/Component.hpp
    include/dusk/Shader.hpp
    include/dusk/Texture.hpp
    include/dusk/Mesh.hpp
    include/dusk/Material.hpp

    # imgui
    include/imgui/imgui.h
    include/imgui/imgui_internal.h
    include/imgui/imgui_impl_glfw_gl3.h

    # glad
    include/glad/glad.h
    include/KHR/khrplatform.h

    # stb
    include/stb/stb_image.h
)

SET(Dusk_SOURCES

    src/dusk/Dusk.cpp
    src/dusk/Util.cpp
    src/dusk/ScriptHost.cpp
    src/dusk/Event.cpp
    src/dusk/EventCallbacks.cpp
    src/dusk/EventDispatcher.cpp
    src/dusk/App.cpp
    src/dusk/UI.cpp
    src/dusk/Scene.cpp
    src/dusk/Camera.cpp
    src/dusk/Actor.cpp
    src/dusk/Component.cpp
    src/dusk/Shader.cpp
    src/dusk/Texture.cpp
    src/dusk/Mesh.cpp
    src/dusk/Material.cpp

    # imgui
    src/imgui/imgui.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_demo.cpp
    src/imgui/imgui_impl_glfw_gl3.cpp

    # glad
    src/glad/glad.c
)

CONFIGURE_FILE(${Dusk_CONFIG_IN} ${Dusk_CONFIG})

ADD_LIBRARY(${Dusk_OUT}
    ${Dusk_CONFIG}
    ${Dusk_INCLUDES}
    ${Dusk_SOURCES}
)
ADD_DEPENDENCIES(${Dusk_OUT} depend)

# Configuration-independant libraries
TARGET_LINK_LIBRARIES(
    ${Dusk_OUT}
    ${Dusk_LINK_LIBRARIES}
)

# Debug-only libraries
FOREACH(debug_lib IN ITEMS ${Dusk_LINK_LIBRARIES_DEBUG})
    TARGET_LINK_LIBRARIES(
        ${Dusk_OUT}
        debug ${debug_lib}
    )
ENDFOREACH()

# Release-only libraries
FOREACH(release_lib IN ITEMS ${Dusk_LINK_LIBRARIES_RELEASE})
    TARGET_LINK_LIBRARIES(
        ${Dusk_OUT}
        optimized ${release_lib}
    )
ENDFOREACH()

# Set IDE folders
FOREACH(file IN ITEMS ${Dusk_ASSETS} ${Dusk_INCLUDES} ${Dusk_SOURCES})
    GET_FILENAME_COMPONENT(file_path ${file} DIRECTORY)
    FILE(TO_NATIVE_PATH ${file_path} file_path)
    SOURCE_GROUP(${file_path} FILES ${file})
ENDFOREACH()

FILE(TO_NATIVE_PATH "include/dusk" config_path)
SOURCE_GROUP(${config_path} FILES ${Dusk_CONFIG})

# Copy assets to build directory
ADD_CUSTOM_TARGET(copy-assets)
ADD_DEPENDENCIES(${Dusk_OUT} copy-assets)
ADD_CUSTOM_COMMAND(
    TARGET copy-assets PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E
        copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
)

SET_TARGET_PROPERTIES(
    copy-assets
    PROPERTIES FOLDER "automation"
)

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

SET_TARGET_PROPERTIES(
    copy-includes
    PROPERTIES FOLDER "automation"
)

### Example projects

ADD_SUBDIRECTORY(examples)
