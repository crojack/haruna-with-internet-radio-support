/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.kirigami as Kirigami
import org.kde.haruna.playlist 1.0

Item {
    id: root

    property var radioModel
    property var mpv
    property int currentPlayingIndex: -1  // Track currently playing station

    Component.onCompleted: {
        // If radioModel has loaded favorites by default, we don't need to spam logs
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Search header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: Kirigami.Theme.alternateBackgroundColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: Kirigami.Units.largeSpacing
                spacing: Kirigami.Units.largeSpacing

                // Search field - expands to fill available space
                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    Layout.minimumWidth: 100
                    Layout.fillHeight: true

                    placeholderText: i18n("Search stations or type 'fav' for favorites")

                    Keys.onReturnPressed: {
                        // Reset playing index when searching
                        root.currentPlayingIndex = -1
                        if (root.radioModel) {
                            root.radioModel.searchStations(text)
                        }
                    }

                    Component.onCompleted: {
                        // Show favorites by default
                        text = "fav"
                        if (root.radioModel) {
                            root.radioModel.searchStations("fav")
                        }
                    }
                }

                // Search button - fixed width, text only
                Button {
                    Layout.preferredWidth: 90
                    Layout.minimumWidth: 70
                    Layout.fillHeight: true
                    text: i18n("Search")

                    onClicked: {
                        // Reset playing index when searching
                        root.currentPlayingIndex = -1
                        if (root.radioModel) {
                            root.radioModel.searchStations(searchField.text)
                        }
                    }
                }
            }
        }

        // Status/info bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.radioModel && root.radioModel.isSearching ? 30 : 0
            color: Kirigami.Theme.positiveBackgroundColor
            visible: root.radioModel && root.radioModel.isSearching

            Label {
                anchors.centerIn: parent
                text: i18n("Searching for stations...")
            }
        }

        // Stations list
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: stationsList
                
                clip: true
                spacing: Kirigami.Units.smallSpacing
                
                model: root.radioModel

                onCountChanged: {
                    // Reset playing index when count changes (new search results)
                    // But only if we aren't just reloading the same list (to preserve selection logic if needed)
                    // root.currentPlayingIndex = -1 
                }
                
                delegate: RadioStationItem {
                    width: stationsList.width
                    isCurrentlyPlaying: root.currentPlayingIndex === index
                    
                    onStationClicked: function(idx) {
                        root.currentPlayingIndex = idx
                        if (root.radioModel) {
                            root.radioModel.playStation(idx)
                        }
                    }

                    onFavoriteToggled: function(idx) {
                        if (root.radioModel) {
                            root.radioModel.toggleFavorite(idx)
                        }
                    }
                }

                // Empty state
                Label {
                    anchors.centerIn: parent
                    visible: stationsList.count === 0 && (!root.radioModel || !root.radioModel.isSearching)
                    text: i18n("No stations found. Try searching for a station name, country, or genre.")
                    color: Qt.rgba(Kirigami.Theme.textColor.r, 
                                  Kirigami.Theme.textColor.g, 
                                  Kirigami.Theme.textColor.b, 0.6)
                    wrapMode: Text.WordWrap
                    width: parent.width * 0.8
                    horizontalAlignment: Text.AlignHCenter
                }

                // Help text when empty and not searching
                Column {
                    anchors.centerIn: parent
                    visible: stationsList.count === 0 && (!root.radioModel || !root.radioModel.isSearching)
                    spacing: Kirigami.Units.largeSpacing
                    width: parent.width * 0.8

                    Label {
                        width: parent.width
                        text: i18n("Search tips:")
                        font.weight: Font.Bold
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Label {
                        width: parent.width
                        text: i18n("• Type station name: 'BBC', 'NPR', 'Radio Paradise'\n• Type country code: 'GB', 'US', 'DE'\n• Type 'genre:jazz' or 'genre:classical'\n• Type 'fav' to show your favorites")
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignLeft
                        color: Qt.rgba(Kirigami.Theme.textColor.r,
                                      Kirigami.Theme.textColor.g,
                                      Kirigami.Theme.textColor.b, 0.7)
                    }
                }
            }
        }
    }

    // Connections to handle playback requests from the model
    Connections {
        target: root.radioModel
        function onPlayStationRequested(url, name) {
            console.log("Playing radio station directly:", name, "URL:", url)
            if (root.mpv && root.mpv.defaultFilterProxyModel) {
                // Use Clear mode to replace playlist content with just this radio station
                root.mpv.defaultFilterProxyModel.addItem(url, PlaylistModel.Clear)
                
                // FIX: Store the station name explicitly so Main.qml uses IT, not the song metadata
                root.mpv.currentRadioStationName = name
                
                // FIX 2: Explicitly force playback to start
                root.mpv.pause = false
                
                console.log("Radio station loaded and playback forced")
            } else {
                console.log("ERROR: mpv or defaultFilterProxyModel is null")
            }
        }
    }
}
