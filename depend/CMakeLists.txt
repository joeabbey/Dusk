
IF(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
    SET(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
ENDIF()

MESSAGE("Adding dependency: glm")
ADD_SUBDIRECTORY(glm)
SET(GLM_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/glm" PARENT_SCOPE)

MESSAGE("Adding dependency: json")
ADD_SUBDIRECTORY(json)
SET(JSON_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/json/src" PARENT_SCOPE)

MESSAGE("Adding dependency: lua")
ADD_SUBDIRECTORY(lua)
SET(LUA_LIBRARIES liblua PARENT_SCOPE)
SET(LUA_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/lua/src" "${CMAKE_BINARY_DIR}/depend/lua" PARENT_SCOPE)

MESSAGE("Adding dependency: zlib")
ADD_SUBDIRECTORY(zlib)
SET(ZLIB_LIBRARIES zlib PARENT_SCOPE)
SET(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/zlib" "${CMAKE_BINARY_DIR}/depend/zlib" PARENT_SCOPE)

MESSAGE("Adding dependency: glfw")
ADD_SUBDIRECTORY(glfw)
SET(GLFW_LIBRARIES glfw PARENT_SCOPE)
SET(GLFW_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/glfw/include")

SET(DEPEND_IGNORE
    glm_dummy
    json_unit catch_main
    lua luac
    zlibstatic example example64 minigzip minigzip64
    glfwinfo heightmap gears boing offscreen particles wave simple splitview reopen monitors iconify gamma
    icon tearing empty events msaa cursor threads clipboard title joysticks sharing windows timeout
)

SET_TARGET_PROPERTIES(
    ${DEPEND_IGNORE}
    PROPERTIES FOLDER "depend")

SET_TARGET_PROPERTIES(
    ${DEPEND_IGNORE}
    PROPERTIES
    EXCLUDE_FROM_ALL 1
    EXCLUDE_FROM_DEFAULT_BUILD 1)
