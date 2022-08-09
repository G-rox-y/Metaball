cmake_minimum_required(VERSION 3.24.0)

project(Metaball)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(SFML_ADDONS)

target_include_directories(${PROJECT_NAME} PUBLIC SFML_ADDONS)

target_link_directories(${PROJECT_NAME} PRIVATE SFML_ADDONS)

target_link_libraries(${PROJECT_NAME} FPSCounter)