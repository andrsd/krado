project(krado-test)

find_package(GTest REQUIRED)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SRCS})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        -DKRADO_UNIT_TESTS_ROOT="${KRADO_UNIT_TESTS_ROOT}"
)

if (KRADO_WITH_MOAB)
    target_compile_definitions(${PROJECT_NAME} PRIVATE KRADO_WITH_MOAB)
endif()

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/lib/include
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        libkrado
        ${OpenCASCADE_DataExchange_LIBRARIES}
        GTest::gmock_main
        fmt::fmt
        exodusIIcpp::exodusIIcpp
)

add_test(
    NAME krado-test
    COMMAND ${PROJECT_NAME}
)

if(KRADO_CODE_COVERAGE)
    set_tests_properties(
        krado-test
        PROPERTIES
            ENVIRONMENT LLVM_PROFILE_FILE=${PROJECT_NAME}.profraw
    )
endif()
