cmake_minimum_required(VERSION 3.16)

if (KRADO_WITH_TRIANGLE AND NOT TRIANGLE_DIR)
    message(FATAL_ERROR "To build with triangle support, set the TRIANGLE_DIR variable to point to the location with triangle source code")
endif ()

if (TRIANGLE_DIR)

project(triangle VERSION 1.6 LANGUAGES C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(${PROJECT_NAME} SHARED ${TRIANGLE_DIR}/triangle.c)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DTRILIBRARY -DANSI_DECLARATORS)

target_include_directories(${PROJECT_NAME} PUBLIC ${TRIANGLE_DIR})

install(
    TARGETS triangle
    EXPORT kradoTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

endif()
