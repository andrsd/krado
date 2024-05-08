set(NAME "krado")

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)

add_library(${NAME} SHARED ${SRCS})

target_code_coverage(${NAME})

set_target_properties(
    ${NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(
    ${NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/lib/include>
        $<INSTALL_INTERFACE:include>
        ${OpenCASCADE_INCLUDE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/include
)

target_link_libraries(
    ${NAME}
    PRIVATE
        ${OpenCASCADE_DataExchange_LIBRARIES}
        fmt::fmt
        Eigen3::Eigen
        kvaredro
)

if (KRADO_WITH_TRIANGLE)
    target_link_libraries(${NAME} PUBLIC triangle)
endif()

configure_file(include/config.h.in include/krado/config.h)
