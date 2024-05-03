set(NAME "krado")

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)

add_library(${NAME} SHARED ${SRCS})

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
    PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/include
)

configure_file(include/config.h.in include/krado/config.h)
