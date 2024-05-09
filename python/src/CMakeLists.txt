find_package(pybind11 2.9 REQUIRED)

pybind11_add_module(krado krado.cpp)

target_include_directories(
    krado
    PRIVATE
        ${CMAKE_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(
    krado
    PUBLIC
        libkrado
)

install(
    TARGETS krado
    COMPONENT python
    LIBRARY DESTINATION "lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages/krado"
)
