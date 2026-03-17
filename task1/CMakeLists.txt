cmake_minimum_required(VERSION 3.16)
project(task1_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AppSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AppContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DataPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ConnectionTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ResourceTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NetworkAddress.cpp
)

add_library(app_core ${APP_SOURCES})

target_include_directories(app_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(net_core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TcpJsonProtocol.cpp
)
target_include_directories(net_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(MyApp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_include_directories(MyApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(MyApp PRIVATE app_core net_core)

add_executable(MyServer ${CMAKE_CURRENT_SOURCE_DIR}/src/server_main.cpp)
target_include_directories(MyServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MyServer PRIVATE app_core net_core)

add_executable(MyClient ${CMAKE_CURRENT_SOURCE_DIR}/src/client_main.cpp)
target_include_directories(MyClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(MyClient PRIVATE app_core net_core)

enable_testing()
add_subdirectory(tests)
