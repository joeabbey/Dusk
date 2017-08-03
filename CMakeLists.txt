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

    CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
    CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX1Y)

    IF(COMPILER_SUPPORTS_CXX14)
        SET(CMAKE_CXX_FLAGS "-std=c++14")
    ELSEIF(COMPILER_SUPPORTS_CXX1Y)
        SET(CMAKE_CXX_FLAGS "-std=c++1y")
    ELSE()
        MESSAGE(FATAL_ERROR "Compiler '${CMAKE_CXX_COMPILER}' has no C++17 support")
    ENDIF()

    ADD_DEFINITIONS(-D_XOPEN_SOURCE=700)
    ADD_COMPILE_OPTIONS(-Wall -Wextra -pedantic -Wno-unused-parameter
        -Wno-gnu-zero-variadic-macro-arguments -Wno-missing-braces
        -Wno-gnu-statement-expression -Wno-keyword-macro -Wno-unused-function)

# VisualStudio
ELSEIF(MSVC)

    ADD_DEFINITIONS(/D_CRT_SECURE_NO_WARNINGS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    SET(CMAKE_EXE_LINKER_FLAGS
        "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")

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
    ${GSL_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${OPENAL_INCLUDE_DIRS}
    ${OGG_INCLUDE_DIRS}
    ${VORBIS_INCLUDE_DIRS}
    ${THEORA_INCLUDE_DIRS}
)

SET(Dusk_LINK_LIBRARIES
    ${OPENGL_LIBRARIES}
    ${LUA_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${FLATBUFFERS_LIBRARIES}
    ${OPENAL_LIBRARIES}
    ${OGG_LIBRARIES}
    ${VORBIS_LIBRARIES}
    ${THEORA_LIBRARIES}
)
SET(Dusk_LINK_LIBRARIES_DEBUG
    ${LUA_LIBRARIES_DEBUG}
    ${ZLIB_LIBRARIES_DEBUG}
    ${GLFW_LIBRARIES_DEBUG}
    ${FLATBUFFERS_LIBRARIES_DEBUG}
    ${OPENAL_LIBRARIES_DEBUG}
    ${OGG_LIBRARIES_DEBUG}
    ${VORBIS_LIBRARIES_DEBUG}
    ${THEORA_LIBRARIES_DEBUG}
)
SET(Dusk_LINK_LIBRARIES_RELEASE
    ${LUA_LIBRARIES_RELEASE}
    ${ZLIB_LIBRARIES_RELEASE}
    ${GLFW_LIBRARIES_RELEASE}
    ${FLATBUFFERS_LIBRARIES_RELEASE}
    ${OPENAL_LIBRARIES_RELEASE}
    ${OGG_LIBRARIES_RELEASE}
    ${VORBIS_LIBRARIES_RELEASE}
    ${THEORA_LIBRARIES_RELEASE}
)

### Dusk library

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include
                    ${CMAKE_BINARY_DIR}/include)

# Allow imgui to #include "imgui.h" and stb sources
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include/imgui
                    ${CMAKE_SOURCE_DIR}/include/stb)

# Disable imconfig.h, stb implementation
ADD_DEFINITIONS(
    -DIMGUI_DISABLE_INCLUDE_IMCONFIG_H
    -DIMGUI_DISABLE_STB_RECT_PACK_IMPLEMENTATION
    -DIMGUI_DISABLE_STB_TRUETYPE_IMPLEMENTATION
)

SET(Dusk_OUT Dusk)

SET(Dusk_CONFIG_IN include/dusk/Config.hpp.in)
SET(Dusk_CONFIG    ${CMAKE_BINARY_DIR}/include/dusk/Config.hpp)

FILE(GLOB_RECURSE Dusk_ASSETS RELATIVE ${CMAKE_SOURCE_DIR} "assets/*")

SET(Dusk_INCLUDES
    #include/dusk/Actor.hpp
    include/dusk/App.hpp
    #include/dusk/Asset.hpp
    include/dusk/Benchmark.hpp
    #include/dusk/Camera.hpp
    #include/dusk/Component.hpp
    include/dusk/Dusk.hpp
    #include/dusk/Event.hpp
    #include/dusk/EventCallbacks.hpp
    #include/dusk/EventDispatcher.hpp
    #include/dusk/Font.hpp
    include/dusk/Log.hpp
    #include/dusk/Material.hpp
    #include/dusk/Mesh.hpp
    #include/dusk/Model.hpp
    #include/dusk/Platform.hpp
    #include/dusk/Scene.hpp
    #include/dusk/ScriptHost.hpp
    include/dusk/Shader.hpp
    include/dusk/Shader.inc.hpp
    #include/dusk/Sound.hpp
    #include/dusk/Texture.hpp
    #include/dusk/Timer.hpp
    #include/dusk/UI.hpp
    include/dusk/Util.hpp
    #include/dusk/Video.hpp

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
    #src/dusk/Actor.cpp
    src/dusk/App.cpp
    #src/dusk/Camera.cpp
    #src/dusk/Component.cpp
    src/dusk/Dusk.cpp
    #src/dusk/Event.cpp
    #src/dusk/EventCallbacks.cpp
    #src/dusk/EventDispatcher.cpp
    #src/dusk/Font.cpp
    #src/dusk/Material.cpp
    #src/dusk/Mesh.cpp
    #src/dusk/Model.cpp
    #src/dusk/Scene.cpp
    #src/dusk/ScriptHost.cpp
    src/dusk/Shader.cpp
    #src/dusk/Sound.cpp
    #src/dusk/Texture.cpp
    #src/dusk/UI.cpp
    src/dusk/Util.cpp
    #src/dusk/Video.cpp

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

SET_TARGET_PROPERTIES(
    ${Dusk_OUT} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

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
ADD_CUSTOM_TARGET(copy-assets ALL)
ADD_DEPENDENCIES(${Dusk_OUT} copy-assets)
ADD_CUSTOM_COMMAND(
    OUTPUT
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
