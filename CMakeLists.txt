cmake_minimum_required(VERSION 3.16)

project(krado VERSION 2.2.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CodeCoverage)

set(BuildValues "Release;Debug;RelWithDebInfo;MinSizeRel")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${BuildValues})
option(KRADO_BUILD_TESTS "Build tests" NO)
option(KRADO_WITH_TRIANGLE "Build with triangle support, set TRIANGLE_DIR to the location with triangle source files" NO)
option(KRADO_WITH_MOAB "Build with MOAB support" NO)

find_package(fmt 11 REQUIRED)
find_package(spdlog 1 REQUIRED)
find_package(exodusIIcpp 3 REQUIRED)
find_package(OpenCASCADE REQUIRED)
if(OpenCASCADE_FOUND)
    message(STATUS "Found OpenCASCADE: ${OpenCASCADE_LIBRARY_DIR} (version ${OpenCASCADE_VERSION})")
    if(OpenCASCADE_VERSION VERSION_LESS 7.7.0)
    endif()
endif()
find_package(Eigen3 3.4 REQUIRED NO_MODULE)
find_package(Boost 1.70 REQUIRED CONFIG)
if (KRADO_WITH_MOAB)
    find_package(MOAB REQUIRED)
endif()

if (KRADO_WITH_TRIANGLE)
    add_subdirectory(contrib/triangle)
endif()
add_subdirectory(contrib/bamg)
add_subdirectory(contrib/robust_predicates)

add_subdirectory(lib)
add_subdirectory(utils)
add_subdirectory(python)

# Tests

if(KRADO_BUILD_TESTS)
    set(KRADO_UNIT_TESTS_ROOT ${CMAKE_SOURCE_DIR}/test CACHE PATH "" FORCE)
    mark_as_advanced(FORCE KRADO_UNIT_TESTS_ROOT)

    enable_testing()
    add_subdirectory(test)
endif()

# install

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/krado-config.cmake.in krado-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/krado
    NO_SET_AND_CHECK_MACRO
)
write_basic_package_version_file(krado-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    TARGETS libkrado
    EXPORT kradoTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/lib/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    EXPORT kradoTargets
    FILE krado-targets.cmake
    NAMESPACE krado::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/krado
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/krado-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/krado-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/krado
)

add_subdirectory(docs)
