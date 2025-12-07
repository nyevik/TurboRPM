cmake_minimum_required(VERSION 3.21)

project(TurboRPM VERSION 0.1 LANGUAGES CXX)

# Qt auto-moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# leave extensions default (GNU) for your Linux-only setup
#set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(turborpm
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/packagemodel.cpp
    src/packagemodel.h
    # Resources
    resources.qrc
)

add_executable(qt_thread_test 
    src/test/qt_thread_test.cpp
    src/test/qtworker.cpp
    src/test/qtworker.h
)

#[[qt_add_resources(turborpm "app_resources"
    PREFIX "/src/icons"
    FILES
        app-icon-512x512.png
)]]

target_link_libraries(turborpm PRIVATE Qt6::Widgets Qt6::Core pthread)

target_link_libraries(qt_thread_test PRIVATE  Qt6::Core pthread)

# Optionally install
#install(TARGETS turborpm)
