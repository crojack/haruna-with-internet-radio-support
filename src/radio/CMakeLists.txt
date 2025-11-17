#
# SPDX-License-Identifier: BSD-3-Clause
#

qt_add_library(radio STATIC)
qt_add_qml_module(radio
    URI org.kde.haruna.radio
    VERSION 1.0
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/radio
    IMPORT_PATH ${CMAKE_BINARY_DIR}
    QML_FILES
        RadioStationItem.qml
        RadioStationsList.qml
    SOURCES
        radiostation.h
        radiostation.cpp
        radiostationsmodel.h
        radiostationsmodel.cpp
)

target_include_directories(radio
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        /opt/kf6/include/KF6/KFileMetaData
        ${CMAKE_SOURCE_DIR}/src/youtube
)

target_link_libraries(radio
    PRIVATE
        playlist

        Qt6::Core
        Qt6::Gui
        Qt6::Network
        Qt6::Quick

        KF6::ConfigCore
        KF6::I18n

        utilitiesplugin
)
