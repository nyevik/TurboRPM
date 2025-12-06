cmake_minimum_required(VERSION 3.21)

project(RpmDnfGui VERSION 0.1 LANGUAGES CXX)

# Qt auto-moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# leave extensions default (GNU) for your Linux-only setup
#set(CMAKE_CXX_EXTENSIONS OFF)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(rpm_dnf_gui
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/packagemodel.cpp
    src/packagemodel.h
)

add_executable(qt_thread_test 
    src/test/qt_thread_test.cpp
    src/test/qtworker.cpp
    src/test/qtworker.h
)
target_link_libraries(rpm_dnf_gui PRIVATE Qt6::Widgets Qt6::Core pthread)

target_link_libraries(qt_thread_test PRIVATE  Qt6::Core pthread)

# Optionally install
#install(TARGETS rpm_dnf_gui)