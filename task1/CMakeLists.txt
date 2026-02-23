cmake_minimum_required(VERSION 3.16)

project(task1_app LANGUAGES CXX)

add_executable(MyApp
    src/main.cpp
    src/utils.cpp
)

target_include_directories(MyApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(MyApp PRIVATE cxx_std_17)