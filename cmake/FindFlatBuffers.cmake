# Copyright 2014 Stefan.Eilemann@epfl.ch
# Copyright 2014 Google Inc. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Find the flatbuffers schema compiler
#
# Output Variables:
# * FLATBUFFERS_FLATC_EXECUTABLE the flatc compiler executable
# * FLATBUFFERS_FOUND
# * FLATBUFFERS_INCLUDE_DIRS
#
# Provides:
# * FLATBUFFERS_GENERATE_C_HEADERS(Name <files>) creates the C++ headers
#   for the given flatbuffer schema files.
#   Returns the header files in ${Name}_OUTPUTS

SET(FLATBUFFERS_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

find_program(
  FLATBUFFERS_FLATC_EXECUTABLE
  NAMES flatc
  PATHS ${FLATBUFFERS_PATH}
  PATH_SUFFIXES bin/amd64 bin
)
FIND_PATH(
  FLATBUFFERS_INCLUDE_DIRS
  NAMES flatbuffers/flatbuffers.h
  PATHS ${FLATBUFFERS_PATH}
  PATH_SUFFIXES include
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
  flatbuffers
  REQUIRED_VARS
  FLATBUFFERS_FLATC_EXECUTABLE
  FLATBUFFERS_INCLUDE_DIRS
)

IF(FLATBUFFERS_FOUND)
  FUNCTION(FLATBUFFERS_GENERATE_C_HEADERS Name)
    SET(FLATC_OUTPUTS)
    FOREACH(FILE ${ARGN})
      GET_FILENAME_COMPONENT(FLATC_OUTPUT ${FILE} NAME_WE)
      SET(
        FLATC_OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/${FLATC_OUTPUT}_generated.h"
      )
      LIST(APPEND FLATC_OUTPUTS ${FLATC_OUTPUT})

      ADD_CUSTOM_COMMAND(OUTPUT ${FLATC_OUTPUT}
        DEPENDS ${FILE}
        COMMAND ${FLATBUFFERS_FLATC_EXECUTABLE}
        ARGS -c -o "${CMAKE_CURRENT_BINARY_DIR}/" ${FILE}
        COMMENT "Building C++ header for ${FILE}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    ENDFOREACH()
    SET(
      ${Name}_OUTPUTS
      ${FLATC_OUTPUTS}
      PARENT_SCOPE
    )
  ENDFUNCTION()

  INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
ENDIF()
