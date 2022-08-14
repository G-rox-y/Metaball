include_directories("../external/SFML/include", ".")

add_library(FPSCounter STATIC FPSCounter.cpp FPSCounter.hpp)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "sfml-system"
        "sfml-window"
        "sfml-graphics"
        )

add_dependencies(FPSCounter ${ADDITIONAL_LIBRARY_DEPENDENCIES})

IF (WIN32)
    target_link_libraries(FPSCounter PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
ELSE()
    target_link_libraries(FPSCounter PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}" X11)
ENDIF()