cmake_minimum_required(VERSION 3.16)
project(BitLA VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 COMPONENTS Widgets REQUIRED)

add_executable(BitLA
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/minerconfigdialog.cpp
    src/minerconfigdialog.h
)

target_link_libraries(BitLA PRIVATE Qt6::Widgets)

target_include_directories(BitLA PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_definitions(BitLA PRIVATE QT_NO_KEYWORDS)
