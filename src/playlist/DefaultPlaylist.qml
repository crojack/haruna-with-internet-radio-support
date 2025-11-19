/*
 * SPDX-FileCopyrightText: 2025
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform as Platform

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.playlist 1.0
import org.kde.haruna.settings
import org.kde.haruna.utilities // Added this import to fix PathUtils error

Item {
    id: root

    property var filterProxyModel
    property var mpv
    property var contextMenuLoader
    property bool isSmallWindowSize: Window.window.width < 600

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Toolbar - 2 rows of 3 buttons each, no gap below tabs
        ToolBar {
            id: toolbar

            Layout.fillWidth: true
            Layout.preferredHeight: visible ? implicitHeight : 0
            visible: PlaylistSettings.showToolbar
            topPadding: 0
            bottomPadding: 0
            
            background: Rectangle {
                color: PlaylistSettings.overlayVideo
                       ? Qt.rgba(Kirigami.Theme.backgroundColor.r,
                                Kirigami.Theme.backgroundColor.g,
                                Kirigami.Theme.backgroundColor.b, 0.3)
                       : Kirigami.Theme.backgroundColor
            }

            // 2x3 Grid: Row 1 (Add, Create, Sort), Row 2 (Playback, Clear, Save As)
            GridLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.smallSpacing
                columns: 3
                rows: 2
                columnSpacing: Kirigami.Units.smallSpacing
                rowSpacing: Kirigami.Units.smallSpacing

                // Row 1: Add, Create, Sort
                Button {
                    text: i18nc("@action:button", "Add")
                    Layout.fillWidth: true
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 1.8
                    onClicked: addMenu.popup()
                    
                    Menu {
                        id: addMenu
                        MenuItem {
                            text: i18nc("@action:button", "Files")
                            onTriggered: {
                                fileDialog.fileType = "video"
                                fileDialog.fileMode = Platform.FileDialog.OpenFiles
                                fileDialog.open()
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "URL")
                            onTriggered: {
                                if (addUrlPopup.opened) {
                                    addUrlPopup.close()
                                } else {
                                    addUrlPopup.open()
                                }
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "Playlist")
                            onTriggered: {
                                fileDialog.fileType = "playlist"
                                fileDialog.fileMode = Platform.FileDialog.OpenFile
                                fileDialog.nameFilters = ["m3u (*.m3u *.m3u8)"]
                                fileDialog.open()
                            }
                        }
                    }
                }

                Button {
                    text: i18nc("@action:button", "Create")
                    Layout.fillWidth: true
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 1.8
                    onClicked: {
                        if (addPlaylistPopup.opened) {
                            addPlaylistPopup.close()
                        } else {
                            addPlaylistPopup.open()
                        }
                    }
                }

                Button {
                    text: i18nc("@action:button", "Sort")
                    Layout.fillWidth: true
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 1.8
                    onClicked: sortMenu.popup()
                    
                    Menu {
                        id: sortMenu
                        MenuItem {
                            text: i18nc("@action:button", "Name, ascending")
                            onTriggered: {
                                root.mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.NameAscending)
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "Name, descending")
                            onTriggered: {
                                root.mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.NameDescending)
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "Duration, ascending")
                            onTriggered: {
                                root.mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.DurationAscending)
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "Duration, descending")
                            onTriggered: {
                                root.mpv.visibleFilterProxyModel.sortItems(PlaylistSortProxyModel.DurationDescending)
                            }
                        }
                    }
                }

                // Row 2: Playback, Clear, Save As
                Button {
                    text: i18nc("@action:button", "Playback")
                    Layout.fillWidth: true
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 1.8
                    onClicked: playbackMenu.popup()
                    
                    Menu {
                        id: playbackMenu
                        MenuItem {
                            text: i18nc("@action:button", "Repeat playlist")
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "RepeatPlaylist"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "RepeatPlaylist"
                                PlaylistSettings.save()
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "Stop after last item")
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "StopAfterLast"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "StopAfterLast"
                                PlaylistSettings.save()
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "Repeat item")
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "RepeatItem"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "RepeatItem"
                                PlaylistSettings.save()
                            }
                        }
                        MenuItem {
                            text: i18nc("@action:button", "Stop after item")
                            checkable: true
                            checked: PlaylistSettings.playbackBehavior === "StopAfterItem"
                            onTriggered: {
                                PlaylistSettings.playbackBehavior = "StopAfterItem"
                                PlaylistSettings.save()
                            }
                        }
                        MenuSeparator {}
                        MenuItem {
                            text: i18nc("@action:button", "Random Playback")
                            checkable: true
                            enabled: ["StopAfterLast", "RepeatPlaylist"].includes(PlaylistSettings.playbackBehavior)
                            checked: PlaylistSettings.randomPlayback
                            onTriggered: {
                                PlaylistSettings.randomPlayback = checked
                                PlaylistSettings.save()
                            }
                        }
                    }
                }

                Button {
                    text: i18nc("@action:button", "Clear")
                    Layout.fillWidth: true
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 1.8
                    onClicked: {
                        root.mpv.visibleFilterProxyModel.clear()
                    }
                }

                Button {
                    text: i18nc("@action:button", "Save As")
                    Layout.fillWidth: true
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 1.8
                    onClicked: {
                        fileDialog.fileType = "playlist"
                        fileDialog.fileMode = Platform.FileDialog.SaveFile
                        fileDialog.nameFilters = ["m3u (*.m3u *.m3u8)"]
                        fileDialog.open()
                    }
                }
            }
        }

        // Search header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: PlaylistSettings.overlayVideo
                   ? "transparent"
                   : Kirigami.Theme.alternateBackgroundColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.largeSpacing
                spacing: Kirigami.Units.largeSpacing

                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    Layout.minimumWidth: 100
                    Layout.fillHeight: true

                    placeholderText: i18n("Search playlist...")

                    onTextChanged: {
                        if (root.filterProxyModel) {
                            root.filterProxyModel.searchText = text
                            playlistView.positionViewAtIndex(0, ListView.Beginning)
                        }
                    }

                    Component.onCompleted: {
                        if (root.filterProxyModel) {
                            text = root.filterProxyModel.searchText || ""
                        }
                    }
                }

                Button {
                    Layout.preferredWidth: 90
                    Layout.minimumWidth: 70
                    Layout.fillHeight: true
                    text: i18n("Search")

                    onClicked: {
                        if (root.filterProxyModel) {
                            root.filterProxyModel.searchText = searchField.text
                        }
                    }
                }
            }
        }

        // Playlist view
        ListView {
            id: playlistView
            
            Layout.fillWidth: true
            Layout.fillHeight: true
            
            model: root.filterProxyModel
            spacing: Kirigami.Units.smallSpacing
            reuseItems: true
            clip: true

            ScrollBar.vertical: ScrollBar {
                id: scrollbar
                policy: ScrollBar.AsNeeded
                z: 100
            }

            delegate: PlaylistItem {
                m_mpv: root.mpv
            }
            
            function openContextMenu(item) {
                root.openContextMenu(item)
            }
        }
    }

    // Popups
    Popup {
        id: addPlaylistPopup

        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        width: 300
        modal: true
        focus: true

        onVisibleChanged: {
            if (visible) {
                playlistName.text = ""
                playlistName.forceActiveFocus()
            }
        }

        ColumnLayout {
            anchors.fill: parent
            Label {
                text: i18nc("@label:textbox", "New playlist name:")
                Layout.fillWidth: true
            }
            TextField {
                id: playlistName
                Layout.fillWidth: true
                placeholderText: i18nc("@info:placeholder", "Enter name")
                onAccepted: {
                    if (text.trim() !== "") {
                        root.mpv.playlists.createNewPlaylist(text.trim())
                        addPlaylistPopup.close()
                    }
                }
            }
            RowLayout {
                Button {
                    text: i18nc("@action:button", "Cancel")
                    onClicked: addPlaylistPopup.close()
                }
                Button {
                    text: i18nc("@action:button", "Add")
                    onClicked: {
                        if (playlistName.text.trim() !== "") {
                            root.mpv.playlists.createNewPlaylist(playlistName.text.trim())
                            addPlaylistPopup.close()
                        }
                    }
                }
            }
        }
    }

    Popup {
        id: addUrlPopup

        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        width: 400
        modal: true
        focus: true

        onVisibleChanged: {
            if (visible) {
                urlField.text = ""
                urlField.forceActiveFocus()
            }
        }

        ColumnLayout {
            anchors.fill: parent
            Label {
                text: i18nc("@label:textbox", "URL or path:")
                Layout.fillWidth: true
            }
            TextField {
                id: urlField
                Layout.fillWidth: true
                placeholderText: i18nc("@info:placeholder", "Enter URL or file path")
                onAccepted: {
                    if (text.trim() !== "") {
                        root.mpv.visibleFilterProxyModel.addItem(text.trim(), PlaylistModel.Append)
                        addUrlPopup.close()
                    }
                }
            }
            RowLayout {
                Button {
                    text: i18nc("@action:button", "Cancel")
                    onClicked: addUrlPopup.close()
                }
                Button {
                    text: i18nc("@action:button", "Add")
                    onClicked: {
                        if (urlField.text.trim() !== "") {
                            root.mpv.visibleFilterProxyModel.addItem(urlField.text.trim(), PlaylistModel.Append)
                            addUrlPopup.close()
                        }
                    }
                }
            }
        }
    }

    Platform.FileDialog {
        id: fileDialog

        property string fileType: "video"

        title: i18nc("@title:window", "Select file")
        folder: GeneralSettings.fileDialogLastLocation
        fileMode: Platform.FileDialog.OpenFile

        onAccepted: {
            switch (fileType) {
            case "video":
                root.mpv.visibleFilterProxyModel.addItems(fileDialog.files, PlaylistModel.Append)
                break
            case "playlist":
                if (fileMode === Platform.FileDialog.OpenFile) {
                    root.mpv.visibleFilterProxyModel.addItem(fileDialog.file, PlaylistModel.Append)
                } else {
                    root.mpv.visibleFilterProxyModel.saveM3uFile(fileDialog.file)
                }

                break
            }
            GeneralSettings.fileDialogLastLocation = PathUtils.parentUrl(fileDialog.file)
            GeneralSettings.save()
        }
        onRejected: root.mpv.focus = true
        onVisibleChanged: {
            HarunaApp.actionsEnabled = !visible
        }
    }
    
    // Expose methods that parent expects
    function positionViewAtIndex(index, mode) {
        playlistView.positionViewAtIndex(index, mode)
    }
    
    function openContextMenu(item) {
        if (root.contextMenuLoader) {
            root.contextMenuLoader.open(item)
        }
    }
}
