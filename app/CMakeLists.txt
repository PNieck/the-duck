add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE duck_lib glfw glad imgui ecs algebra stb_image)
