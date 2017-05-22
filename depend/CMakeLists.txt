
IF(BUILD_DEPENDENCIES)

    # GLEW ?

    # ZLIB ?

    # FlatBuffers ?

ENDIF()

# GLM
LIST(APPEND DEPEND_PREFIX ${CMAKE_CURRENT_LIST_DIR}/glm)

# Prepend in-repo dependencies to library search path
SET(CMAKE_PREFIX_PATH
    ${DEPEND_PREFIX}
    ${CMAKE_PREFIX_PATH})
