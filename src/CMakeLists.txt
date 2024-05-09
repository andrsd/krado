set(NAME "krado-bin")

add_executable(${NAME}
    main.cpp
)

set_target_properties(${NAME} PROPERTIES OUTPUT_NAME krado)

target_include_directories(${NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/lib/include>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_SOURCE_DIR}/lib/include
        ${CMAKE_SOURCE_DIR}/contrib
)

target_link_libraries(${NAME}
    PRIVATE
        libkrado
)
