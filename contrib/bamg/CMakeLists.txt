cmake_minimum_required(VERSION 3.16)

project(libbamg LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SRC bamglib/*.cpp *.cpp)
list(REMOVE_ITEM SRC ${PROJECT_SOURCE_DIR}/bamglib/Meshgibbs.cpp)
list(APPEND SRC bamg.cpp)

add_library(${PROJECT_NAME} SHARED ${SRC})

if(NOT MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME bamg)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC -DCMAKE -DNOTFREEFEM)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/bamglib
        ${PROJECT_SOURCE_DIR}/fflib
)
