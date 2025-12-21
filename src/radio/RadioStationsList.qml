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
    property int currentPlayingIndex: -1

    Component.onCompleted: {

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

                    onStationClicked: function(idx, stationTags) {
                        root.currentPlayingIndex = idx
                        if (root.radioModel) {
                            // Store tags in mpv before playing
                            if (root.mpv) {
                                root.mpv.currentRadioStationTags = stationTags
                                console.log("[RadioStationsList] Stored tags for genre matching:", stationTags)
                            }
                            root.radioModel.playStation(idx)
                        }
                    }

                    onFavoriteToggled: function(idx) {
                        if (root.radioModel) {
                            root.radioModel.toggleFavorite(idx)
                        }
                    }
                }

                // Unified Empty State & Help Container
                // Groups the error message and tips so they don't overlap
                Column {
                    anchors.centerIn: parent
                    width: parent.width * 0.8
                    spacing: Kirigami.Units.largeSpacing
                    visible: stationsList.count === 0 && (!root.radioModel || !root.radioModel.isSearching)

                    // 1. No stations found message
                    Label {
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        text: i18n("No stations found. Try searching for a station name, country, or genre.")
                        color: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.6)
                        wrapMode: Text.WordWrap
                    }

                    // 2. Spacer
                    Item { 
                        width: 1 
                        height: Kirigami.Units.gridUnit 
                    }

                    // 3. Search Tips Header
                    Label {
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        text: i18n("Search tips:")
                        font.weight: Font.Bold
                    }

                    // 4. Search Tips Body
                    Label {
                        width: parent.width
                        text: i18n("• Type station name:  'BBC', 'NPR', 'Radio Paradise'\n• Type country code:  'GB', 'US', 'DE'\n• Type genre name:  'jazz' or 'classical'\n• Type artist name:  'Mozart' or 'Beatles'\n• Type  'fav'  to show your favorites")
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignLeft
                        color: Qt.rgba(Kirigami.Theme.textColor.r, Kirigami.Theme.textColor.g, Kirigami.Theme.textColor.b, 0.7)
                    }
                }
            }
        }
    }
}
