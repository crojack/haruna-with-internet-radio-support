/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 * SPDX-FileCopyrightText: 2025 Muhammet Sadık Uğursoy <sadikugursoy@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.playlist
import org.kde.haruna.utilities
import org.kde.haruna.settings
import org.kde.haruna.youtube
import org.kde.haruna.radio 1.0

Page {
    id: root

    required property MpvVideo m_mpv

    property bool isSmallWindowSize: Window.window.width < 600
    property int buttonSize: isSmallWindowSize ? Kirigami.Units.iconSizes.small : Kirigami.Units.iconSizes.smallMedium
    property alias scrollPositionTimer: scrollPositionTimer
    property real customWidth: PlaylistSettings.playlistWidth
    property real fsScale: Window.window.isFullScreen() && PlaylistSettings.bigFontFullscreen ? 1.36 : 1
    property int previousTabIndex: 0  // Track previous tab for cleanup
    
    // FIX 1: Variable to remember the playing radio station index when switching tabs
    property int savedRadioIndex: -1

    width: limitWidth(customWidth) * fsScale

    function limitWidth(pWidth) {
        if (PlaylistSettings.style === "compact") {
            return 380
        } else {
            return Math.min(Math.max(pWidth, 260), Window.window.width - 50)
        }
    }

    x: PlaylistSettings.position === "right" ? Window.window.width : -width
    y: 0
    padding: 0
    state: PlaylistSettings.rememberState
           ? (PlaylistSettings.visible ? "visible" : "hidden")
           : "hidden"

    onStateChanged: {
        PlaylistSettings.visible = state === "visible" ? true : false
        PlaylistSettings.save()
        if (state === "hidden") {
            contextMenuLoader.active = false
        }
    }
    
    onWidthChanged: {
        if (PlaylistSettings.overlayVideo && state === "visible") {
            refreshTimer.restart()
        }
    }
    
    onHeightChanged: {
        if (PlaylistSettings.overlayVideo && state === "visible") {
            refreshTimer.restart()
        }
    }

    component ResizeHandler: Item {
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            cursorShape: Qt.SizeHorCursor

            drag {
                target: parent
                axis: Drag.XAxis
                threshold: 0
            }

            onPositionChanged: {
                if (!drag.active) {
                    return
                }

                if (PlaylistSettings.position === "right") {
                    let mX = root.m_mpv.mapFromItem(this, mouseX, mouseY).x
                    var w = root.limitWidth(Window.window.width - mX)
                } else {
                    let mX = playlistContentItem.mapFromItem(this, mouseX, mouseY).x
                    var w = root.limitWidth(mX)
                }
                root.customWidth = w / root.fsScale
            }

            onReleased: {
                PlaylistSettings.playlistWidth = root.customWidth
                PlaylistSettings.save()
            }
        }
    }

    Rectangle {
        id: playlistEdgeBorder

        x: PlaylistSettings.position === "left"
                ? parent.width - width
                : 0
        y: 0
        z: 30
        width: 1
        height: root.height
        color: Kirigami.Theme.backgroundColor

        ResizeHandler {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 8
            height: parent.height
        }

        Rectangle {
            id: dragHandle

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 5
            height: 50
            color: Kirigami.Theme.alternateBackgroundColor
            radius: Kirigami.Units.cornerRadius
            border {
                width: 1
                color: Kirigami.Theme.backgroundColor
            }

            ResizeHandler {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 18
                height: parent.height
            }
        }
    }

    // Background blur effect
    ShaderEffectSource {
        id: shaderEffect

        visible: PlaylistSettings.overlayVideo
        anchors.fill: parent
        anchors {
            leftMargin: playlistEdgeBorder.width
            rightMargin: playlistEdgeBorder.width
        }
        sourceItem: root.m_mpv
        sourceRect: {
            if (PlaylistSettings.position === "right") {
                return Qt.rect(
                    root.x,
                    root.m_mpv.y,
                    root.width,
                    root.height
                    )
            }
            else {
                return Qt.rect(
                    root.x,
                    0,
                    root.width,
                    root.height
                    )
            }
        }
        z: 5
    }

    FastBlur {
        id: blurEffect
        
        visible: PlaylistSettings.overlayVideo
        anchors.fill: shaderEffect
        radius: 100
        source: shaderEffect
        z: 10
    }

    Timer {
        id: refreshTimer
        interval: 100
        running: false
        repeat: false
        onTriggered: {
            if (PlaylistSettings.overlayVideo && shaderEffect.visible && root.state === "visible") {
                shaderEffect.scheduleUpdate()
            }
        }
    }
    
    Connections {
        target: PlaylistSettings
        function onOverlayVideoChanged() {
            if (PlaylistSettings.overlayVideo && root.state === "visible") {
                refreshTimer.restart()
            }
        }
    }

    Rectangle {
        id: mainContentRect
        
        anchors.fill: parent
        anchors {
            leftMargin: playlistEdgeBorder.width
            rightMargin: playlistEdgeBorder.width
        }
        color: PlaylistSettings.overlayVideo 
               ? "transparent" 
               : Kirigami.Theme.backgroundColor
        z: 20

        ColumnLayout {
            id: mainLayout
            anchors.fill: parent
            spacing: Kirigami.Units.smallSpacing

            // Tabs
            TabBar {
                id: playlistTabView

                Layout.fillWidth: true
                Layout.preferredHeight: 40
                implicitWidth: 0

                background: Rectangle {
                    color: PlaylistSettings.overlayVideo
                           ? Qt.rgba(Kirigami.Theme.backgroundColor.r,
                                    Kirigami.Theme.backgroundColor.g,
                                    Kirigami.Theme.backgroundColor.b, 0.3)
                           : Kirigami.Theme.backgroundColor
                }

                onCurrentIndexChanged: {
                    root.m_mpv.playlists.visibleIndex = currentIndex
                    root.previousTabIndex = currentIndex
                }

                Repeater {
                    model: root.m_mpv.playlists
                    delegate: PlaylistTabDelegate {
                        m_mpv: root.m_mpv
                    }
                }

                function openContextMenu(tab) {
                    tab = tab as PlaylistTabDelegate
                    if (!tab) {
                        return
                    }
                    tabMenuLoader.open(tab)
                }

                function movePlaylistItem(from, to) {
                    moveItem(from, to)
                    var contextMenu = tabMenuLoader.item as Menu

                    if (!contextMenu) {
                        return
                    }

                    if (contextMenu.count > 3) {
                        contextMenu.moveItem(from + 3, to + 3)
                    }
                }

                Connections {
                    target: root.m_mpv.playlists
                    function onVisibleIndexChanged() {
                        playlistTabView.currentIndex = root.m_mpv.playlists.visibleIndex
                    }
                }
            }

            // Content area
            Item {
                id: playlistContentItem
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.topMargin: Kirigami.Units.smallSpacing

                DropArea {
                    id: playlistDropArea

                    anchors.fill: parent
                    keys: ["text/uri-list"]

                    onDropped: drop => {
                        if (!containsDrag) {
                            return
                        }
                        root.m_mpv.visibleFilterProxyModel.addFilesAndFolders(drop.urls, PlaylistModel.Append)
                    }
                }

                Loader {
                    id: playlistLoader
                    anchors.fill: parent
                    sourceComponent: isRadioTab ? radioComponent : playlistComponent

                    property bool isRadioTab: {
                        if (!root.m_mpv || !root.m_mpv.visibleFilterProxyModel) {
                            return false
                        }
                        return root.m_mpv.visibleFilterProxyModel.playlistName === "Internet Radio"
                    }

                    function getPlaylistView() {
                        return playlistLoader.item
                    }
                }

                Component {
                    id: radioComponent
                    RadioStationsList {
                        radioModel: root.m_mpv.radioStationsModel
                        mpv: root.m_mpv
                        
                        // FIX 1: Bind index to parent property so it survives tab switching
                        currentPlayingIndex: root.savedRadioIndex
                        onCurrentPlayingIndexChanged: root.savedRadioIndex = currentPlayingIndex
                    }
                }

                Component {
                    id: playlistComponent
                    DefaultPlaylist {
                        filterProxyModel: root.m_mpv.visibleFilterProxyModel
                        mpv: root.m_mpv
                        contextMenuLoader: contextMenuLoader
                    }
                }
            }
        }
    }

    Loader {
        id: tabMenuLoader

        property int row: -1

        active: true
        asynchronous: false
        sourceComponent: Menu {
            id: tabMenu
            MenuItem {
                text: i18nc("@action:inmenu", "Rename")
                visible: tabMenuLoader.row > 0
                onClicked: root.m_mpv.playlists.renamePlaylist(tabMenuLoader.row)
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Remove")
                visible: tabMenuLoader.row > 0
                onClicked: root.m_mpv.playlists.removePlaylist(tabMenuLoader.row)
            }

            MenuSeparator {
                visible: tabMenuLoader.row > 0
            }

            Instantiator {
                model: root.m_mpv.playlists
                delegate: MenuItem {
                    required property int index
                    required property string name
                    required property bool isVisible
                    required property bool isActive

                    text: name
                    onClicked: root.m_mpv.playlists.visibleIndex = index
                }
                onObjectAdded: function (index, object) {
                    tabMenu.insertItem(index + 3, object)
                }
                onObjectRemoved: function (index, object) {
                    tabMenu.removeItem(object)
                }
            }
        }

        function open(tab) {
            const theTab = tab as PlaylistTabDelegate
            if (!theTab) {
                return
            }
            row = theTab.index
            const menu = item as Menu
            menu.popup()
        }
    }

    Loader {
        id: contextMenuLoader

        property int row: -1
        property bool isLocal: false

        active: false
        asynchronous: true
        sourceComponent: Menu {

            MenuItem {
                text: i18nc("@action:inmenu", "Open Containing Folder")
                visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                onClicked: root.m_mpv.visibleFilterProxyModel.highlightInFileManager(contextMenuLoader.row)
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Open in Browser")
                visible: !contextMenuLoader.isLocal && contextMenuLoader.row != -1
                onClicked: {
                    const modelIndex = root.m_mpv.visibleFilterProxyModel.index(contextMenuLoader.row, 0)
                    Qt.openUrlExternally(modelIndex.data(PlaylistModel.PathRole))
                }
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Copy Name")
                onClicked: root.m_mpv.visibleFilterProxyModel.copyFileName(contextMenuLoader.row)
                visible: contextMenuLoader.row != -1
            }
            MenuItem {
                text: contextMenuLoader.isLocal
                      ? i18nc("@action:inmenu", "Copy Path")
                      : i18nc("@action:inmenu", "Copy URL")
                onClicked: root.m_mpv.visibleFilterProxyModel.copyFilePath(contextMenuLoader.row)
                visible: contextMenuLoader.row != -1
            }
            MenuSeparator {
                visible: contextMenuLoader.row != -1
            }

            MenuItem {
                text: i18nc("@action:inmenu", "Select All")
                onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, PlaylistFilterProxyModel.All)
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Deselect All")
                onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, PlaylistFilterProxyModel.Clear)
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Invert Selection")
                onClicked: root.m_mpv.visibleFilterProxyModel.selectItem(0, PlaylistFilterProxyModel.Invert)
            }
            MenuSeparator {
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Remove From Playlist")
                visible: root.m_mpv.visibleFilterProxyModel.selectionCount > 1 && contextMenuLoader.row != -1
                onClicked: root.m_mpv.visibleFilterProxyModel.removeItems()
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Remove From Playlist")
                visible: root.m_mpv.visibleFilterProxyModel.selectionCount === 1 && contextMenuLoader.row != -1
                onClicked: root.m_mpv.visibleFilterProxyModel.removeItem(contextMenuLoader.row)
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Rename")
                visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
                onClicked: root.m_mpv.visibleFilterProxyModel.renameFile(contextMenuLoader.row)
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Scroll to Playing Item")
                onClicked: {
                    const index = root.m_mpv.visibleFilterProxyModel.getPlayingItem()
                    playlistLoader.item?.positionViewAtIndex(index, ListView.Beginning)
                }
            }
            MenuSeparator {
                visible: contextMenuLoader.isLocal && contextMenuLoader.row != -1
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Move File to Trash")
                visible: contextMenuLoader.isLocal && root.m_mpv.visibleFilterProxyModel.selectionCount === 1 && contextMenuLoader.row != -1
                onClicked: root.m_mpv.visibleFilterProxyModel.trashFile(contextMenuLoader.row)
            }
            MenuItem {
                text: i18nc("@action:inmenu", "Move Selected Files to Trash")
                visible: contextMenuLoader.isLocal && root.m_mpv.visibleFilterProxyModel.selectionCount > 1 && contextMenuLoader.row != -1
                onClicked: root.m_mpv.visibleFilterProxyModel.trashFiles()
            }
        }

        function open(item: PlaylistItemDelegate) : void {
            if (!contextMenuLoader.active) {
                contextMenuLoader.active = true
                contextMenuLoader.loaded.connect(function() {
                    contextMenuLoader.open(item)
                })
                return
            }

            if (item) {
                contextMenuLoader.row = item.index
                contextMenuLoader.isLocal = item.isLocal
            }
            else {
                contextMenuLoader.row = -1
            }

            const contextMenu = contextMenuLoader.item as Menu
            contextMenu.popup()
        }
    }

    Timer {
        id: scrollPositionTimer

        interval: 100
        running: false
        repeat: false

        onTriggered: {
            playlistLoader.item?.positionViewAtIndex(root.m_mpv.visibleFilterProxyModel.playingVideo, ListView.Beginning)
        }
    }

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
                        root.m_mpv.playlists.createNewPlaylist(text.trim())
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
                            root.m_mpv.playlists.createNewPlaylist(playlistName.text.trim())
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
                        root.m_mpv.visibleFilterProxyModel.addItem(text.trim(), PlaylistModel.Append)
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
                            root.m_mpv.visibleFilterProxyModel.addItem(urlField.text.trim(), PlaylistModel.Append)
                            addUrlPopup.close()
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog

        property string fileType: "video"

        title: i18nc("@title:window", "Select file")
        currentFolder: GeneralSettings.fileDialogLastLocation
        fileMode: FileDialog.OpenFile
        nameFilters: {
            if (fileType === "playlist" ) {
                return ["m3u (*.m3u *.m3u8)"]
            } else {
                return []
            }
        }

        onAccepted: {
            switch (fileType) {
            case "video":
                root.m_mpv.visibleFilterProxyModel.addItems(fileDialog.selectedFiles, PlaylistModel.Append)
                break
            case "playlist":
                if (fileMode === FileDialog.OpenFile) {
                    root.m_mpv.visibleFilterProxyModel.addItem(fileDialog.selectedFile, PlaylistModel.Append)
                } else {
                    root.m_mpv.visibleFilterProxyModel.saveM3uFile(fileDialog.selectedFile)
                }

                break
            }
            GeneralSettings.fileDialogLastLocation = PathUtils.parentUrl(fileDialog.selectedFile)
            GeneralSettings.save()
        }
        onRejected: root.m_mpv.focus = true
        onVisibleChanged: {
            HarunaApp.actionsEnabled = !visible
        }
    }

    states: [
        State {
            name: "hidden"

            PropertyChanges {
                root.x: PlaylistSettings.position === "right"
                        ? root.Window.window.width
                        : -width
                root.visible: false
            }
        },
        State {
            name : "visible"

            PropertyChanges {
                root.x: PlaylistSettings.position === "right"
                        ? root.Window.window.width - root.width
                        : 0
                root.visible: true
            }
        }
    ]

    transitions: [
        Transition {
            from: "visible"
            to: "hidden"

            SequentialAnimation {
                NumberAnimation {
                    target: root
                    property: "x"
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.InQuad
                }

                PropertyAction {
                    target: root
                    property: "visible"
                    value: false
                }
            }
        },
        Transition {
            from: "hidden"
            to: "visible"

            SequentialAnimation {
                PropertyAction {
                    target: root
                    property: "visible"
                    value: true
                }

                NumberAnimation {
                    target: root
                    property: "x"
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutQuad
                }
            }
        }
    ]
}
