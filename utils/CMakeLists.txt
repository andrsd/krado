set(KRADO_INFO "krado-info")

add_executable(${KRADO_INFO} info.cpp)

set_target_properties(${KRADO_INFO} PROPERTIES OUTPUT_NAME ${KRADO_INFO})

target_include_directories(${KRADO_INFO}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/lib/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_SOURCE_DIR}/lib/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(${KRADO_INFO}
    PRIVATE
        libkrado
        fmt::fmt
)

install(
    TARGETS ${KRADO_INFO}
    EXPORT kradoTargets
    RUNTIME DESTINATION bin
)
