cmake_minimum_required(VERSION 3.16)

project(task1_app LANGUAGES CXX)



add_library(app_core
    src/utils.cpp
    src/AppSettings.cpp
    src/AppContext.cpp
    src/DataPool.cpp
    src/ConnectionTest.cpp
    src/ResourceTest.cpp
)

add_executable(MyApp
    src/main.cpp

)

target_include_directories(MyApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(MyApp PRIVATE app_core)

enable_testing()
add_subdirectory(tests)


target_compile_features(MyApp PRIVATE cxx_std_17)