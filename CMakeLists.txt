cmake_minimum_required(VERSION 3.16)
project(RavenEYE_DSA_Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(${PROJECT_NAME}
        main.cpp
        mainwindow.h   # ensure automoc processes it
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
