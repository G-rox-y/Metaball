cmake_minimum_required(VERSION 3.24.0)

project(Metaball)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(SFML_ADDONS)

add_subdirectory(external/SFML)

target_include_directories(${PROJECT_NAME}
    PUBLIC SFML_ADDONS
    PUBLIC external/SFML/include
)

target_link_directories(${PROJECT_NAME}
    PRIVATE SFML_ADDONS
    PRIVATE external/SFML/lib
)

target_link_libraries(${PROJECT_NAME}
    FPSCounter 
    "${ADDITIONAL_LIBRARY_DEPENDENCIES}"
)
