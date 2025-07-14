set(NAME "libkrado")

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)

add_library(${NAME} SHARED ${SRCS})
if(NOT MSVC)
    set_target_properties(${NAME} PROPERTIES OUTPUT_NAME krado)
endif()

target_code_coverage(${NAME})

set_target_properties(
    ${NAME}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_compile_definitions(
    ${NAME}
    PRIVATE
        SPDLOG_FMT_EXTERNAL
)

target_include_directories(
    ${NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/lib/include>
        $<INSTALL_INTERFACE:include>
        ${OpenCASCADE_INCLUDE_DIR}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/include>
        ${CMAKE_SOURCE_DIR}/contrib
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(
    ${NAME}
    PRIVATE
        Eigen3::Eigen
        libbamg
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        exodusIIcpp::exodusIIcpp
        ${Boost_LIBRARIES}
        ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
        ${OpenCASCADE_DataExchange_LIBRARIES}
)

target_compile_definitions(${NAME} PRIVATE SPDLOG_HEADER_ONLY)

if (KRADO_WITH_TRIANGLE)
    target_link_libraries(${NAME} PUBLIC triangle)
endif()

if (KRADO_WITH_MOAB)
    target_link_libraries(${NAME} PRIVATE MOAB)
    target_compile_definitions(${NAME} PRIVATE KRADO_WITH_MOAB)
endif()

configure_file(include/config.h.in include/krado/config.h)
