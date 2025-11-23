/*
 * SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.platform 1.1

import org.kde.kirigami as Kirigami
import org.kde.config as KConfig

import org.kde.haruna
import org.kde.haruna.playlist 1.0
import org.kde.haruna.utilities
import org.kde.haruna.settings
import org.kde.haruna.youtube
import org.kde.haruna.radio 1.0

ApplicationWindow {
    id: window

    property bool containsMouse: false

    property int previousVisibility: Window.Windowed
    property var acceptedSubtitleTypes: ["application/x-subrip", "text/x-ssa", "text/x-microdvd"]

    visible: true
    title: mpv.mediaTitle || i18nc("@title:window", "Haruna")
    width: 1000
    height: 600
    minimumWidth: 400
    minimumHeight: 200
    color: Kirigami.Theme.backgroundColor
    
    // Apply custom application font if set
    font: {
        if (GeneralSettings.applicationFontFamily !== "") {
            return Qt.font({
                family: GeneralSettings.applicationFontFamily,
                pointSize: GeneralSettings.applicationFontSize,
                bold: GeneralSettings.applicationFontBold,
                italic: GeneralSettings.applicationFontItalic
            })
        }
        return Qt.application.font
    }
    

    onVisibilityChanged: function(visibility) {
        if (PlaybackSettings.pauseWhileMinimized) {
            if (visibility === Window.Minimized) {
                if (mpv.pause) {
                    mpv.preMinimizePlaybackState = MpvVideo.PlaybackState.Paused
                } else {
                    mpv.preMinimizePlaybackState = MpvVideo.PlaybackState.Playing
                }
                mpv.pause = true
            }
            if (previousVisibility === Window.Minimized
                    && visibility === Window.Windowed | Window.Maximized | Window.FullScreen) {
                if (mpv.preMinimizePlaybackState === MpvVideo.PlaybackState.Playing) {
                    mpv.pause = false
                }
            }
        }

        // used to restore window state, when exiting fullscreen,
        // to the one it had before going fullscreen
        if (visibility !== Window.FullScreen) {
            previousVisibility = visibility
        }
    }

    onClosing: {
        const settingsWindow = settingsLoader.item as Window
        settingsWindow?.close()
    }

    header: Header {
        id: header

        m_mpv: mpv
        m_menuBarLoader: menuBarLoader
        m_recentFilesModel: recentFilesModel
        m_settingsLoader: settingsLoader
    }

    menuBar: MenuBarLoader {
        id: menuBarLoader

        m_mpv: mpv
        m_recentFilesModel: recentFilesModel
        m_settingsLoader: settingsLoader
    }

    Connections {
        target: GeneralSettings
        function onOsdFontSizeChanged() {
            osd.message("Test osd font size")
        }
        function onMaxRecentFilesChanged() {
            recentFilesModel.getItems()
        }
        function onResizeWindowToVideoChanged() {
            window.resizeWindow()
        }
        function onColorSchemeChanged() {
            HarunaApp.activateColorScheme(GeneralSettings.colorScheme)
        }
    }
    Connections {
        target: AudioSettings
        function onPreferredTrackChanged() {
            mpv.audioId = AudioSettings.preferredTrack === 0
                    ? "auto"
                    : AudioSettings.preferredTrack
        }
        function onPreferredLanguageChanged() {
            mpv.setProperty(MpvProperties.AudioLanguage, AudioSettings.preferredLanguage.replace(/\s+/g, ''))
        }
        function onReplayGainChanged() {
            mpv.setProperty(MpvProperties.ReplayGain, AudioSettings.replayGain)
        }
        function onReplayGainPreampChanged() {
            mpv.setProperty(MpvProperties.ReplayGainPreamp, AudioSettings.replayGainPreamp)
        }
        function onReplayGainPreventClipChanged() {
            mpv.setProperty(MpvProperties.ReplayGainClip, !AudioSettings.replayGainPreventClip)
        }
        function onReplayGainFallbackChanged() {
            mpv.setProperty(MpvProperties.ReplayGainFallback, AudioSettings.replayGainFallback)
        }
    }
    Connections {
        target: PlaybackSettings
        function onYtdlFormatChanged() {
            mpv.setProperty(MpvProperties.YtdlFormat, PlaybackSettings.ytdlFormat)
        }
        function onHWDecodingChanged() {
            mpv.setProperty(MpvProperties.HardwareDecoding, PlaybackSettings.hWDecoding)
        }
    }
    Connections {
        target: SubtitlesSettings
        function onAutoSelectSubtitlesChanged() {
            mpv.selectSubtitleTrack()
        }
        function onPreferredLanguageChanged() {
            mpv.setProperty(MpvProperties.SubtitleLanguage, SubtitlesSettings.preferredLanguage.replace(/\s+/g, ''))
        }
        function onPreferredTrackChanged() {
            mpv.subtitleId = SubtitlesSettings.preferredTrack === 0
                    ? "auto"
                    : SubtitlesSettings.preferredTrack
        }
        function onAllowOnBlackBordersChanged() {
            mpv.setProperty(MpvProperties.SubtitleUseMargins, SubtitlesSettings.allowOnBlackBorders ? "yes" : "no")
            mpv.setProperty(MpvProperties.SubtitleAssForceMargins, SubtitlesSettings.allowOnBlackBorders ? "yes" : "no")
        }
        function onFontFamilyChanged() {
            mpv.setProperty(MpvProperties.SubtitleFont, SubtitlesSettings.fontFamily)
        }
        function onFontSizeChanged() {
            mpv.setProperty(MpvProperties.SubtitleFontSize, SubtitlesSettings.fontSize)
        }
        function onIsBoldChanged() {
            mpv.setProperty(MpvProperties.SubtitleBold, SubtitlesSettings.isBold)
        }
        function onIsItalicChanged() {
            mpv.setProperty(MpvProperties.SubtitleItalic, SubtitlesSettings.isItalic)
        }
        function onFontColorChanged() {
            mpv.setProperty(MpvProperties.SubtitleColor, SubtitlesSettings.fontColor)
        }
        function onShadowColorChanged() {
            mpv.setProperty(MpvProperties.SubtitleShadowColor, SubtitlesSettings.shadowColor)
        }
        function onShadowOffsetChanged() {
            mpv.setProperty(MpvProperties.SubtitleShadowOffset, SubtitlesSettings.shadowOffset)
        }
        function onBorderColorChanged() {
            mpv.setProperty(MpvProperties.SubtitleBorderColor, SubtitlesSettings.borderColor)
        }
        function onBorderSizeChanged() {
            mpv.setProperty(MpvProperties.SubtitleBorderSize, SubtitlesSettings.borderSize)
        }
    }
    Connections {
        target: VideoSettings
        function onScreenshotTemplateChanged() {
            mpv.setProperty(MpvProperties.ScreenshotTemplate, VideoSettings.screenshotTemplate)
        }
        function onScreenshotFormatChanged() {
            mpv.setProperty(MpvProperties.ScreenshotFormat, VideoSettings.screenshotFormat)
        }
    }

    Loader {
        active: false
        sourceComponent: KConfig.WindowStateSaver {
            configGroupName: "MainWindow"
        }
        Component.onCompleted: active = GeneralSettings.rememberWindowGeometry
    }

    MpvVideo {
        id: mpv

        osd: osd
        mouseActionsModel: mouseActionsModel
        radioStationsModel: radioStationsModel

        property string currentRadioStationName: ""
        property string currentRadioStationTags: ""

        width: window.contentItem.width
        height: window.isFullScreen()
                ? window.contentItem.height
                : window.contentItem.height - (footer.isFloating ? 0 : footer.height)
        anchors.left: PlaylistSettings.overlayVideo
                      ? window.contentItem.left
                      : (PlaylistSettings.position === "left" ? playlist.right : window.contentItem.left)
        anchors.right: PlaylistSettings.overlayVideo
                       ? window.contentItem.right
                       : (PlaylistSettings.position === "right" ? playlist.left : window.contentItem.right)
        anchors.top: window.contentItem.top

        onVideoReconfig: window.resizeWindow()

        onAddToRecentFiles: function(url, openedFrom, name) {
            recentFilesModel.addRecentFile(url, openedFrom, name)
        }
        
        // AUTOMATIC CROP LOGIC for Embedded Art
        // If we are playing an audio file, we force MPV to crop (fill) the screen
        onCurrentUrlChanged: {
             if (isAudioFile(currentUrl)) {
                 // panscan 1.0 = Crop/Fill (PreserveAspectCrop)
                 mpv.setProperty("panscan", 1.0)
             } else {
                 // panscan 0.0 = Fit (PreserveAspectFit) - Default for movies
                 mpv.setProperty("panscan", 0.0)
             }
        }
        
        // Improved Helper: Handles extensions AND network streams (http/https)
        function isAudioFile(path) {
            if (!path) return false
            const p = path.toString().toLowerCase()
            
            // 1. Check standard audio extensions
            if (p.endsWith(".mp3") || p.endsWith(".flac") || 
                   p.endsWith(".m4a") || p.endsWith(".ogg") || 
                   p.endsWith(".opus") || p.endsWith(".wav") || 
                   p.endsWith(".wma") || p.endsWith(".aac")) {
                return true
            }
            
            // 2. Check for network streams that are NOT video files
            // This handles radio streams like "http://.../stream_128"
            if ((p.startsWith("http://") || p.startsWith("https://")) && 
                !p.endsWith(".mkv") && !p.endsWith(".mp4") && !p.endsWith(".avi") && 
                !p.endsWith(".webm") && !p.endsWith(".mov")) {
                return true
            }
            
            return false
        }

        // Intelligent Image Overlay System
        Rectangle {
            id: imageOverlayContainer

            anchors.fill: parent
            color: Kirigami.Theme.backgroundColor

            // Track what type of image we're trying to load
            property int imageAttemptStage: 0  // 0=station logo, 2=default (1=genre removed)
            property string lastStationName: ""
            property string lastStationTags: ""

            // Consolidate image logic
            property string currentImageSource: {
                // FIX: Qt.labs.platform StandardPaths.writableLocation returns a URL (file://...)
                // We should NOT prepend 'file://' again.
                var rawPath = StandardPaths.writableLocation(StandardPaths.AppDataLocation) + "/images/"

                console.log("====== IMAGE OVERLAY DEBUG START ======")
                console.log("[currentImageSource] Raw path:", rawPath)

                // Normalize path: Ensure it ends in slash and is treated as a string for concat
                if (rawPath.toString().endsWith("/") === false) {
                    rawPath += "/"
                }

                // Helper to combine path
                function toUrl(pathFragment) {
                    return rawPath + pathFragment
                }

                // Case 1: Idle (No file loaded) - Show Default Background
                if (!mpv.currentUrl || mpv.currentUrl.toString() === "") {
                    console.log("[currentImageSource] Case 1: Idle - showing default background")
                    imageAttemptStage = 2  // Skip to default
                    return toUrl("background/background.jpg")
                }

                // Case 2: Radio stream
                if (mpv.visibleFilterProxyModel && mpv.visibleFilterProxyModel.playlistName === "Internet Radio") {
                    console.log("[currentImageSource] Case 2: Radio stream detected")

                    // FIX: Use the explicitly stored station name, NOT mediaTitle (which changes with songs)
                    let stationName = mpv.currentRadioStationName || mpv.mediaTitle || ""
                    let stationTags = mpv.currentRadioStationTags || ""

                    // Store for later use in error handler
                    lastStationName = stationName
                    lastStationTags = stationTags

                    console.log("[currentImageSource] Station name:", stationName)
                    console.log("[currentImageSource] Station tags:", stationTags)
                    console.log("[currentImageSource] Media title:", mpv.mediaTitle)
                    console.log("[currentImageSource] Current attempt stage:", imageAttemptStage)

                    // Stage 0: Try specific station logo
                    if (imageAttemptStage === 0) {
                        console.log("[currentImageSource] Stage 0: Trying specific station logo...")
                        let stationLogo = getStationLogo(stationName)
                        if (stationLogo !== "") {
                            console.log("[currentImageSource] Attempting station logo:", stationLogo)
                            console.log("[currentImageSource] Full URL:", toUrl(stationLogo))
                            return toUrl(stationLogo)
                        }
                        console.log("[currentImageSource] No station logo path generated, skipping genre and going to default...")
                        imageAttemptStage = 2  // Skip genre (1) and go to default (2)
                    }

                    // Stage 1: Genre logo logic REMOVED

                    // Stage 2: Default radio logo
                    console.log("[currentImageSource] Stage 2: Using default radio logo")
                    console.log("[currentImageSource] Full URL:", toUrl("radio-stations-logos/radio-default.png"))
                    return toUrl("radio-stations-logos/radio-default.png")
                }

                // Case 3: Audio file WITH embedded art (Handled by MPV panscan)
                // Return empty so overlay hides

                // Case 4: Audio file without album art (No video stream detected)
                if (mpv.videoWidth === 0 && mpv.videoHeight === 0) {
                    console.log("[currentImageSource] Case 4: Audio file without art - showing default music logo")
                    imageAttemptStage = 2  // Skip to default
                    return toUrl("default-album-art/music-default.png")
                }

                console.log("[currentImageSource] No overlay needed - video or embedded art")
                console.log("====== IMAGE OVERLAY DEBUG END ======")
                return ""
            }

            // Visibility Logic
            visible: {
                if (!mpv.currentUrl || mpv.currentUrl.toString() === "") return true // Idle

                    if (mpv.visibleFilterProxyModel && mpv.visibleFilterProxyModel.playlistName === "Internet Radio") return true // Radio

                        if (mpv.videoWidth === 0 && mpv.videoHeight === 0) return true // Audio only

                            if (mpv.videoWidth > 0 && mpv.isAudioFile(mpv.currentUrl)) return false // Embedded art (MPV shows it)

                                return false // Regular video
            }

            onCurrentImageSourceChanged: {
                console.log("[imageOverlayContainer] Image source changed to:", currentImageSource)
                imageOverlay.source = Qt.binding(function() { return currentImageSource })
            }

            // Reset attempt stage when station changes
            Connections {
                target: mpv
                function onCurrentRadioStationNameChanged() {
                    console.log("[imageOverlayContainer] Station changed, resetting to stage 0")
                    imageOverlayContainer.imageAttemptStage = 0
                }
            }

            // SINGLE IMAGE COMPONENT
            Image {
                id: imageOverlay

                anchors.fill: parent
                source: imageOverlayContainer.currentImageSource

                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: false

                onStatusChanged: {
                    console.log("[Image] Status changed to:", status, "for source:", source)

                    if (status === Image.Loading) {
                        console.log("[Image] Loading image...")
                    } else if (status === Image.Ready) {
                        console.log("[Image] ✓ Successfully loaded image")
                    } else if (status === Image.Error) {
                        console.log("[Image] ✗ ERROR: Failed to load image from:", source)

                        // Fallback Logic for Radio
                        if (mpv.visibleFilterProxyModel && mpv.visibleFilterProxyModel.playlistName === "Internet Radio") {
                            if (imageOverlayContainer.imageAttemptStage === 0) {
                                // Station logo failed, skip genre and go to default
                                console.log("[Image] Station logo failed, skipping genre, advancing to stage 2 (default)")
                                imageOverlayContainer.imageAttemptStage = 2
                            } else {
                                // Even default failed!
                                console.log("[Image] ERROR: Even default radio logo failed to load!")
                            }
                        }
                    }
                }
            }

            // Helper: Returns RELATIVE path fragment for station logo
            // Tries multiple extensions to find existing files
            function getStationLogo(stationName) {
                console.log("[getStationLogo] Called with station name:", stationName)

                if (!stationName) {
                    console.log("[getStationLogo] Empty station name, returning empty")
                    return ""
                }

                let normalized = stationName.toLowerCase()
                .trim()
                .replace(/[^a-z0-9]+/g, '_')
                .replace(/^_+|_+$/g, '')

                console.log("[getStationLogo] Normalized name:", normalized)

                if (normalized.length === 0) {
                    console.log("[getStationLogo] Normalized name is empty, returning empty")
                    return ""
                }

                // Return first attempt - .jpeg (we'll try other extensions if this fails)
                let logoPath = "radio-stations-logos/" + normalized + ".jpeg"
                console.log("[getStationLogo] Returning path:", logoPath)
                return logoPath
            }

            // Genre Logo Helper REMOVED
        }

        Osd {
            id: osd
            active: mpv.isReady
            maxWidth: mpv.width
            z: 10 // Explicitly set higher Z to ensure OSD is always on top of the overlay
        }

        SelectActionPopup {
            id: triggerActionPopup
            onActionSelected: function(actionName) {
                HarunaApp.actions[actionName].trigger()
            }
        }
    }


    // extra space outside the playlist so that the playlist is not closed
    // when the mouse leaves it by mistake (dragging items, resizing the playlist)
    Item {
        // when window width is very small, there is only 50 pixels not covered by the playlist
        // in that case the extra space is reduced to 30 to allow the playlist to be closed with the mouse
        width: playlist.width >= Window.window.width - 70 ? 30 : 50
        height: playlist.height
        anchors.right: PlaylistSettings.position === "right" ? playlist.left : undefined
        anchors.left: PlaylistSettings.position === "left" ? playlist.right : undefined
        visible: playlist.visible
        HoverHandler {}
    }

    Playlist {
        id: playlist

        m_mpv: mpv
        height: mpv.height
        z: 5 

        Connections {
            target: actions
            function onTogglePlaylist() {
                if (playlist.state === "visible") {
                    playlist.state = "hidden"
                } else {
                    playlist.state = "visible"
                }
            }
        }

        Connections {
            target: HarunaApp
            function onQmlApplicationMouseLeave() {
                if (PlaylistSettings.canToggleWithMouse && (window.isFullScreen() || window.isMaximized())) {
                    playlist.state = "hidden"
                }
            }
        }

        Connections {
            target: mpv
            function onOpenPlaylist() {
                playlist.state = "visible"
            }
            function onClosePlaylist() {
                playlist.state = "hidden"
            }
        }
    }

    Footer {
        id: footer

        anchors.bottom: window.contentItem.bottom

        m_mpv: mpv
        m_menuBarLoader: menuBarLoader
        m_header: header
        m_recentFilesModel: recentFilesModel
        m_settingsLoader: settingsLoader
    }

    Actions {
        id: actions

        m_actionsModel: actionsModel
        m_mpv: mpv
        m_mpvContextMenuLoader: mpvContextMenuLoader
        m_osd: osd
        m_settingsLoader: settingsLoader
        m_triggerActionPopup: triggerActionPopup
        m_openUrlPopup: openUrlPopup

        onOpenFileDialog: fileDialog.open()
        onOpenSubtitleDialog: subtitlesFileDialog.open()
    }

    MouseActionsModel {
        id: mouseActionsModel
    }

    ActionsModel {
        id: actionsModel
    }

    ProxyActionsModel {
        id: proxyActionsModel

        sourceModel: actionsModel
    }

    CustomCommandsModel {
        id: customCommandsModel

        appActionsModel: actionsModel
        Component.onCompleted: init()
    }

    RecentFilesModel {
        id: recentFilesModel
    }

    RadioStationsModel {
        id: radioStationsModel
    }

    RowLayout {
        width: window.width * 0.8 > Kirigami.Units.gridUnit * 50
               ? Kirigami.Units.gridUnit * 50
               : window.width * 0.8
        anchors.centerIn: parent

        Kirigami.InlineMessage {
            id: messageBox

            Layout.fillWidth: true
            Layout.fillHeight: true
            type: Kirigami.MessageType.Error
            showCloseButton: true

            Connections {
                target: MiscUtils
                function onError(message) {
                    messageBox.visible = true
                    messageBox.text = message
                }
            }
        }
    }

    Loader {
        id: mpvContextMenuLoader

        active: false
        asynchronous: true
        sourceComponent: MpvContextMenu {
            m_mpv: mpv
            onClosed: mpvContextMenuLoader.active = false
        }

        function openMpvContextMenuLoader() : void {
            if (!mpvContextMenuLoader.active) {
                mpvContextMenuLoader.active = true
                mpvContextMenuLoader.loaded.connect(function() {
                    openMpvContextMenuLoader()
                })
                return
            }

            const contextMenu = mpvContextMenuLoader.item as MpvContextMenu
            contextMenu.popup()
        }

        function closeMpvContextMenuLoader() : void {
            mpvContextMenuLoader.active = false
        }
    }

    Loader {
        id: settingsLoader

        property int page: SettingsWindow.Page.General

        active: false
        asynchronous: true
        sourceComponent: SettingsWindow {
            m_mpv: mpv
            m_proxyActionsModel: proxyActionsModel
            m_customCommandsModel: customCommandsModel
            m_mouseActionsModel: mouseActionsModel

            onClosing: settingsLoader.active = false
            onCurrentPageChanged: settingsLoader.page = currentPage
        }

        function openSettingPage(page: int) : void {
            if (!settingsLoader.active) {
                settingsLoader.active = true
                settingsLoader.loaded.connect(function() {
                    settingsLoader.openSettingPage(page)
                })
                return
            }

            const settingsWindow = settingsLoader.item as SettingsWindow
            settingsWindow.currentPage = page
            if (settingsWindow.visible) {
                settingsWindow.raise()
            } else {
                settingsWindow.visible = true
            }
        }
    }

    Connections {
        target: HarunaApp
        function onQmlApplicationMouseLeave() {
            window.containsMouse = false
        }
        function onQmlApplicationMouseEnter() {
            window.containsMouse = true
        }
        function onOpenUrl(url) {
            if (GeneralSettings.appendVideoToSingleInstance) {
                let behavior = GeneralSettings.playNewFileInSingleInstance
                    ? PlaylistModel.AppendAndPlay
                    : PlaylistModel.Append

                mpv.defaultFilterProxyModel.addItem(url.toString(), behavior)
                return
            }

            window.openFile(url, RecentFilesModel.OpenedFrom.ExternalApp)
        }
    }

    FileDialog {
        id: fileDialog

        title: i18nc("@title:window", "Select File")
        folder: GeneralSettings.fileDialogLastLocation
        fileMode: FileDialog.OpenFile

        onAccepted: {
            window.openFile(fileDialog.file, RecentFilesModel.OpenedFrom.OpenAction)
            mpv.focus = true

            GeneralSettings.fileDialogLastLocation = PathUtils.parentUrl(fileDialog.file)
            GeneralSettings.save()
        }
        onRejected: mpv.focus = true
        onVisibleChanged: {
            HarunaApp.actionsEnabled = !visible
        }
    }

    FileDialog {
        id: subtitlesFileDialog

        title: i18nc("@title:window", "Select Subtitles File")
        folder: PathUtils.parentUrl(mpv.currentUrl)
        fileMode: FileDialog.OpenFile
        nameFilters: ["Subtitles (*.srt *.ssa *.ass *.sub)"]

        onAccepted: {
            if (window.acceptedSubtitleTypes.includes(MiscUtils.mimeType(subtitlesFileDialog.file))) {
                mpv.addSubtitles(subtitlesFileDialog.file)
            }
        }
        onRejected: mpv.focus = true
        onVisibleChanged: {
            HarunaApp.actionsEnabled = !visible
        }
    }

    YouTube {
        id: youtube
    }

    InputPopup {
        id: openUrlPopup

        x: 10
        y: 10
        width: Math.min(window.width * 0.9, 600)
        lastText: GeneralSettings.lastUrl
        buttonText: i18nc("@action:button", "Open")
        warningText: youtube.hasYoutubeDl()
                     ? ""
                     : i18nc("@info", "Neither <a href=\"https://github.com/yt-dlp/yt-dlp\">yt-dlp</a> nor <a href=\"https://github.com/ytdl-org/youtube-dl\">youtube-dl</a> was found.")

        onSubmitted: function(url) {
            window.openFile(youtube.normalizeUrl(url), RecentFilesModel.OpenedFrom.OpenAction)

            GeneralSettings.lastText = url
            GeneralSettings.save()
        }
    }

    function playlistExists(name) {
        for (let i = 0; i < mpv.playlists.count; ++i) {
            const playlist = mpv.playlists.get(i)
            if (playlist && playlist.name === name) {
                return true
            }
        }
        return false
    }

    function selectPlaylistByName(name) {
        for (let i = 0; i < mpv.playlists.count; ++i) {
            const playlist = mpv.playlists.get(i)
            if (playlist && playlist.name === name) {
                mpv.playlists.visibleIndex = i
                return
            }
        }
    }

    Component.onCompleted: {
        HarunaApp.activateColorScheme(GeneralSettings.colorScheme)

        const hasCommandLineFile = HarunaApp.url(0).toString() !== ""
        const hasLastPlayedFile = GeneralSettings.lastPlayedFile !== ""
        const hasFileToOpen = hasCommandLineFile || (PlaybackSettings.openLastPlayedFile && hasLastPlayedFile)
        if (GeneralSettings.fullscreenOnStartUp && hasFileToOpen) {
            toggleFullScreen()
        }
    }

    function openFile(path: string, openedFrom: int) : void {
        recentFilesModel.addRecentFile(path, openedFrom)
        mpv.defaultFilterProxyModel.addItem(path, PlaylistModel.Clear)
    }

    function isFullScreen() : bool {
        return window.visibility === Window.FullScreen
    }

    function isMaximized() : bool {
        return window.visibility === Window.Maximized
    }

    function toggleFullScreen() : void {
        if (!isFullScreen()) {
            window.showFullScreen()
        } else {
            exitFullscreen()
        }
    }

    function exitFullscreen() : void {
        if (window.previousVisibility === Window.Maximized) {
            window.show()
            window.showMaximized()
        } else {
            window.showNormal()
        }
    }

    function resizeWindow() : void {
        if (SystemUtils.isPlatformWayland() || !GeneralSettings.resizeWindowToVideo || isFullScreen()) {
            return
        }

        window.width = mpv.videoWidth
        window.height = mpv.videoHeight
                + (footer.isFloating ? 0 : footer.height)
                + (header.visible ? header.height : 0)
                + (menuBar.visible ? menuBar.height : 0)
    }
}
