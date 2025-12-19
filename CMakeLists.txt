#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

cmake_minimum_required(VERSION 3.16...3.31)

cmake_policy(SET CMP0048 NEW)
project(haruna VERSION 1.6.0 LANGUAGES CXX)

set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src)
set(QML_IMPORT_PATH ${QT_QML_OUTPUT_DIRECTORY} CACHE PATH "Extra QML import paths to make Qt Creator happy")

set(QT_MIN_VERSION 6.6.0)
set(KF6_MIN_VERSION 6.0.0)

include(FeatureSummary)

# --- QT6 & ECM INITIALIZATION ---
find_package(ECM ${KF6_MIN_VERSION} NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED
    URL "https://api.kde.org/ecm/"
    DESCRIPTION "extra cmake modules")
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(DBUS_PLATFORMS Linux FreeBSD GNU/kFreeBSD NetBSD OpenBSD)

# Consolidated Qt6 find_package call to prevent "Targets Defined: Core" errors
set(HARUNA_QT_COMPONENTS
    Core
    Core5Compat
    Gui
    Qml
    Quick
    QuickControls2
    Sql
)

if (CMAKE_SYSTEM_NAME IN_LIST DBUS_PLATFORMS)
    list(APPEND HARUNA_QT_COMPONENTS DBus)
    set(HAVE_DBUS TRUE)
else()
    set(HAVE_DBUS FALSE)
endif ()

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS ${HARUNA_QT_COMPONENTS})

# --- CONFIGURATION ---
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/config-haruna.h.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/src/config-haruna.h
)

# --- DEPENDENCIES ---
find_package(MpvQt)
set_package_properties(MpvQt PROPERTIES TYPE REQUIRED
    URL "https://invent.kde.org/libraries/mpvqt")

find_package(FFmpeg)
set_package_properties(FFmpeg PROPERTIES TYPE REQUIRED
    URL "https://ffmpeg.org"
    DESCRIPTION "needed for playlist and mpris thumbnails")

find_package(Breeze)
set_package_properties(Breeze PROPERTIES TYPE RECOMMENDED
    URL "https://api.kde.org/frameworks/breeze-icons/html/index.html")

# --- KDE FRAMEWORKS 6 ---
find_package(KF6ColorScheme ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6Config ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6CoreAddons ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6FileMetaData ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6I18n ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6IconThemes ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6KIO ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6Kirigami ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6WindowSystem ${KF6_MIN_VERSION} REQUIRED)
find_package(KF6Crash ${KF6_MIN_VERSION} REQUIRED)

find_package(KDSingleApplication-qt6 REQUIRED)
set_package_properties(KDSingleApplication-qt6 PROPERTIES TYPE REQUIRED
    URL "https://github.com/KDAB/KDSingleApplication"
    DESCRIPTION "helper class for single-instance policy applications")

find_package(Ytdlp)
set_package_properties(Ytdlp PROPERTIES TYPE RUNTIME
    URL "https://github.com/yt-dlp/yt-dlp"
    DESCRIPTION "needed to play videos from youtube and other online sources")

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# --- KDE SETTINGS & TOOLS ---
include(KDEInstallDirs)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
#include(KDEClangFormat)
include(KDEGitCommitHooks)
include(ECMSetupVersion)

# Tooling Configuration
#file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.hpp *.c)
#kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX HARUNA
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/haruna-version.h"
)

ki18n_install(po)

# --- SUBDIRECTORIES ---
add_subdirectory(data)
add_subdirectory(src)
