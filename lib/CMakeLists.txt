file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HDRS CONFIGURE_DEPENDS include/krado/*.h)

add_library(libkrado SHARED)

target_sources(libkrado PRIVATE ${SRCS})

target_sources(
    libkrado
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_SOURCE_DIR}/lib/include
        FILES ${HDRS}
)

target_compile_features(libkrado PUBLIC cxx_std_20)

if(NOT MSVC)
    set_target_properties(libkrado PROPERTIES OUTPUT_NAME krado)
endif()

target_code_coverage(libkrado)

set_target_properties(
    libkrado
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_compile_definitions(
    libkrado
    PRIVATE
        SPDLOG_FMT_EXTERNAL
    PUBLIC
        KRADO_VERSION="${PROJECT_VERSION}"
)

if (KRADO_WITH_TRIANGLE)
    target_compile_definitions(libkrado PUBLIC KRADO_WITH_TRIANGLE)
endif()

target_include_directories(
    libkrado
    PUBLIC
        $<INSTALL_INTERFACE:include>
        ${OpenCASCADE_INCLUDE_DIR}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/include>
        ${CMAKE_SOURCE_DIR}/contrib
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(
    libkrado
    PRIVATE
        Eigen3::Eigen
        libbamg
        robust_predicates
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        exodusIIcpp::exodusIIcpp
        ${Boost_LIBRARIES}
        ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
        ${OpenCASCADE_DataExchange_LIBRARIES}
)

target_compile_definitions(libkrado PRIVATE SPDLOG_HEADER_ONLY)

if (KRADO_WITH_TRIANGLE)
    target_link_libraries(libkrado PUBLIC triangle)
endif()

if (KRADO_WITH_MOAB)
    target_link_libraries(libkrado PRIVATE MOAB)
    target_compile_definitions(libkrado PRIVATE KRADO_WITH_MOAB)
endif()

# install

install(
    TARGETS libkrado
    EXPORT krado-targets
    FILE_SET HEADERS
)
