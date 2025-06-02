find_package(pybind11 2.9 REQUIRED)

pybind11_add_module(krado krado.cpp)

target_include_directories(
    krado
    PRIVATE
        ${CMAKE_BINARY_DIR}/lib/include
        ${CMAKE_SOURCE_DIR}/lib/include
)

target_link_libraries(krado PUBLIC libkrado)

set(PYTHON_SITE lib/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages)

install(
    TARGETS krado
    COMPONENT python
    LIBRARY DESTINATION "${PYTHON_SITE}/krado"
)

install(
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/krado/__init__.py
    DESTINATION ${PYTHON_SITE}/krado
    COMPONENT python
)
