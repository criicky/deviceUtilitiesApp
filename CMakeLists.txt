cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Core QuickControls2)

qt_standard_project_setup()

qt_add_executable(appuntitled
    main.cpp
    pippo.cpp
)

qt_add_qml_module(appuntitled
    URI untitled
    VERSION 1.0
    QML_FILES Main.qml
)

configure_file("AndroidManifest.xml" ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)

set(qtNS /home/cricky/qtdevice/lib)

add_library(${androidx_core_LIBRARY} SHARED IMPORTED)

message(WARNING "ALOOO" ${CMAKE_SOURCE_DIR})

set_target_properties(appuntitled PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    QT_ANDROID_EXTRA_LIBS ${qtNS}/libQt6NetworkSettings_x86_64.so
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android/
)

include_directories(
    /home/cricky/untitled/android/src
    /home/cricky/untitled/android
    /home/cricky/untitled
    /home/cricky/qtdevice
    /home/cricky/qtdevice/include
    /home/cricky/qtdevice/include/QtNetworkSettings
)

link_directories(
    /home/cricky/untitled/android
    /home/cricky/untitled/android/src
    /home/cricky/untitled
    /home/cricky/qtdevice
    /home/cricky/qtdevice/include
    /home/cricky/qtdevice/include/QtNetworkSettings
)

target_link_directories(appuntitled PRIVATE
    /home/cricky/untitled/android
    /home/cricky/untitled/android/src
    /home/cricky/untitled
    /home/cricky/qtdevice
    /home/cricky/qtdevice/lib
)

target_link_libraries(appuntitled PRIVATE
    Qt6::Quick
    Qt6NetworkSettings_x86_64
)

install(TARGETS appuntitled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_BINARY_DIR}
)
