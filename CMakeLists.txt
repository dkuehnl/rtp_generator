cmake_minimum_required(VERSION 3.16)

project(RTP-Generator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

#PJSIP-Configuration:
set(PJSIP_DIR "C:/Users/dkueh/Workspace/cpp/third_party_libs/pjproject-2.15.1")

include_directories(
  ${PJSIP_DIR}/pjlib/include
  ${PJSIP_DIR}/pjlib-util/include
  ${PJSIP_DIR}/pjsip/include
  ${PJSIP_DIR}/pjmedia/include
  ${PJSIP_DIR}/pjnath/include
)

add_compile_definitions(PJ_WIN32=1)

link_directories(
  ${PJSIP_DIR}/lib
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RTP-Generator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        sipmachine.h sipmachine.cpp
        siplogwriter.h siplogwriter.cpp
        flowchart.h flowchart.cpp
        sipcall.h sipcall.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RTP-Generator APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RTP-Generator SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RTP-Generator
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(RTP-Generator
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
  PRIVATE Qt${QT_VERSION_MAJOR}::Network
  ${PJSIP_DIR}/lib/libpjproject-x86_64-x64-vc14-Debug-Dynamic.lib
  ws2_32
  ole32
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RTP-Generator)
endif()
set_target_properties(RTP-Generator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RTP-Generator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RTP-Generator)
endif()
