#
# Algebra library
#
file(GLOB_RECURSE ALGEBRA_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/algebra/*.hpp")
file(GLOB_RECURSE ALGEBRA_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/algebra/*.cpp")

add_library(algebra ${ALGEBRA_LIB_SRCS} ${ALGEBRA_LIB_HEADERS})
target_include_directories(algebra PUBLIC ../include)


#
# ECS library
#
file(GLOB_RECURSE ECS_LIB_HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/ecs/*.hpp")
file(GLOB_RECURSE ECS_LIB_SRCS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/ecs/*.cpp")

add_library(ecs ${ECS_LIB_SRCS} ${ECS_LIB_HEADERS})
target_include_directories(ecs PUBLIC ../include)


#
# Main library
#
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/duck/*.hpp")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/duck/*.cpp")

add_library(duck_lib ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(duck_lib PUBLIC ../include)
target_link_libraries(duck_lib PRIVATE glfw glad imgui ecs algebra stb_image)

# Copy shaders
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
)
add_dependencies(duck_lib copy_shaders)

# Copy textures
add_custom_target(copy_textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/textures ${CMAKE_BINARY_DIR}/textures
)
add_dependencies(duck_lib copy_textures)


# For IDEs
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST} ${ECS_LIB_HEADERS}
)
