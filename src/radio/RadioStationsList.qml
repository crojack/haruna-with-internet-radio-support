/*
 * SPDX-FileCopyrightText: 2025 
 *
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
        console.log("=== RadioStationsList DEBUG ===")
        console.log("RadioStationsList loaded")
        console.log("radioModel:", root.radioModel)
        console.log("radioModel type:", typeof root.radioModel)
        console.log("mpv:", root.mpv)
        console.log("mpv type:", typeof root.mpv)
        
        if (root.radioModel) {
            console.log("radioModel exists!")
            console.log("radioModel.rowCount:", root.radioModel.rowCount)
            console.log("radioModel.favoriteCount:", root.radioModel.favoriteCount)
            console.log("radioModel.isSearching:", root.radioModel.isSearching)
        } else {
            console.log("ERROR: radioModel is NULL or undefined!")
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Debug info bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "darkred"
            visible: !root.radioModel

            Label {
                anchors.centerIn: parent
                text: "DEBUG: radioModel is " + (root.radioModel ? "VALID" : "NULL/UNDEFINED")
                color: "white"
                font.bold: true
            }
        }

        // Search header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: Kirigami.Theme.alternateBackgroundColor

            Component.onCompleted: {
                console.log("Search header Rectangle loaded")
            }

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
                        console.log("Search Enter pressed, text:", text)
                        // Reset playing index when searching
                        root.currentPlayingIndex = -1
                        if (root.radioModel) {
                            console.log("Calling radioModel.searchStations with:", text)
                            root.radioModel.searchStations(text)
                        } else {
                            console.log("ERROR: Cannot search, radioModel is null")
                        }
                    }

                    Component.onCompleted: {
                        console.log("SearchField loaded, placeholderText:", placeholderText)
                        console.log("SearchField Component.onCompleted - loading favorites")
                        // Show favorites by default
                        text = "fav"
                        if (root.radioModel) {
                            console.log("Calling searchStations('fav') to load favorites")
                            root.radioModel.searchStations("fav")
                        } else {
                            console.log("ERROR: Cannot load favorites, radioModel is null")
                        }
                    }
                }

                // Search button - fixed width, text only
                Button {
                    Layout.preferredWidth: 90
                    Layout.minimumWidth: 70
                    Layout.fillHeight: true
                    text: i18n("Search")

                    Component.onCompleted: {
                        console.log("Search Button loaded, text:", text)
                    }

                    onClicked: {
                        console.log("Search button clicked, searchField.text:", searchField.text)
                        // Reset playing index when searching
                        root.currentPlayingIndex = -1
                        if (root.radioModel) {
                            console.log("Calling radioModel.searchStations with:", searchField.text)
                            root.radioModel.searchStations(searchField.text)
                        } else {
                            console.log("ERROR: Cannot search, radioModel is null")
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

            Component.onCompleted: {
                console.log("ScrollView loaded")
            }

            ListView {
                id: stationsList
                
                clip: true
                spacing: Kirigami.Units.smallSpacing
                
                model: root.radioModel

                Component.onCompleted: {
                    console.log("StationsList ListView loaded")
                    console.log("ListView model:", model)
                    console.log("ListView count:", count)
                }

                onCountChanged: {
                    console.log("StationsList count changed to:", count)
                    // Reset playing index when count changes (new search results)
                    root.currentPlayingIndex = -1
                }
                
                delegate: RadioStationItem {
                    width: stationsList.width
                    isCurrentlyPlaying: root.currentPlayingIndex === index
                    
                    onStationClicked: function(idx) {
                        console.log("Station clicked at index:", idx)
                        root.currentPlayingIndex = idx
                        if (root.radioModel) {
                            root.radioModel.playStation(idx)
                        }
                    }

                    onFavoriteToggled: function(idx) {
                        console.log("Favorite toggled at index:", idx)
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
                    
                    Component.onCompleted: {
                        console.log("Empty state label, visible:", visible)
                    }
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
                // This effectively plays the station without accumulating items in the playlist
                root.mpv.defaultFilterProxyModel.addItem(url, PlaylistModel.Clear)
                console.log("Radio station loaded (playlist cleared and replaced with single station)")
            } else {
                console.log("ERROR: mpv or defaultFilterProxyModel is null")
            }
        }
    }
}
