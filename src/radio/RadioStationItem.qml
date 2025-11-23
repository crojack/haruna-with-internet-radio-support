/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.kirigami as Kirigami

ItemDelegate {
    id: root

    // Properties bound from RadioStationsList.qml delegate
    required property int index
    required property string name
    required property string url
    required property string tags
    required property int bitrate
    required property bool isFavorite
    required property string favicon

    // Property to indicate if this station is currently playing
    property bool isCurrentlyPlaying: false

    signal favoriteToggled(int index)
    signal stationClicked(int index, string stationTags)  // CHANGED: Added tags parameter

    width: ListView.view ? ListView.view.width : 0
    height: 60

    background: Rectangle {
        // Add slight background tint when playing
        color: root.isCurrentlyPlaying
        ? Qt.rgba(0, 0.83, 1, 0.08)  // Very subtle blue tint
        : (root.hovered ? Qt.rgba(1, 1, 1, 0.1) : "transparent")
        border.width: 1  // Always 1 pixel
        border.color: root.isCurrentlyPlaying ? "#00D4FF" : Qt.rgba(1, 1, 1, 0.2)
        radius: 4

        Behavior on color {
            ColorAnimation { duration: 150 }
        }

        Behavior on border.color {
            ColorAnimation { duration: 150 }
        }
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing
        anchors.fill: parent
        anchors.margins: Kirigami.Units.smallSpacing

        // Favorite star button - using Unicode star for guaranteed color control
        Item {
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            Layout.alignment: Qt.AlignVCenter

            // Unicode star with full color control
            Text {
                id: starText
                anchors.centerIn: parent
                text: root.isFavorite ? "★" : "☆"
                font.pixelSize: 28
                font.family: "Sans Serif"
                // Always show correct color based on isFavorite property
                color: root.isFavorite ? "#FFD700" : Qt.rgba(0.6, 0.6, 0.6, 0.9)

                Behavior on color {
                    ColorAnimation { duration: 150 }
                }

                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }

            MouseArea {
                id: favoriteMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor

                onClicked: {
                    mouse.accepted = true
                    root.favoriteToggled(root.index)
                }

                onEntered: {
                    starText.scale = 1.2
                    // Don't change color on hover - let isFavorite control it
                }

                onExited: {
                    starText.scale = 1.0
                    // Don't change color on exit - let isFavorite control it
                }

                onPressed: {
                    starText.scale = 0.9
                }

                onReleased: {
                    starText.scale = favoriteMouseArea.containsMouse ? 1.2 : 1.0
                }
            }

            ToolTip {
                visible: favoriteMouseArea.containsMouse
                text: root.isFavorite ? i18n("Remove from favorites") : i18n("Add to favorites")
                delay: 1000
            }
        }

        // Station info column
        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 2

            // Station name
            Text {
                text: root.name
                color: root.isCurrentlyPlaying ? "#00D4FF" : Kirigami.Theme.textColor
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.1
                font.weight: root.isCurrentlyPlaying ? Font.Bold : Font.Medium
                elide: Text.ElideRight
                Layout.fillWidth: true

                Behavior on color {
                    ColorAnimation { duration: 150 }
                }
            }

            // Tags
            Label {
                Layout.fillWidth: true
                text: root.tags
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.9
                color: Qt.rgba(Kirigami.Theme.textColor.r,
                               Kirigami.Theme.textColor.g,
                               Kirigami.Theme.textColor.b, 0.7)
                elide: Text.ElideRight
                visible: root.tags !== ""
            }
        }

        // Bitrate label
        Label {
            Layout.preferredWidth: 80
            Layout.alignment: Qt.AlignVCenter
            text: root.bitrate > 0 ? root.bitrate + " kbps" : ""
            font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.9
            color: Qt.rgba(Kirigami.Theme.textColor.r,
                           Kirigami.Theme.textColor.g,
                           Kirigami.Theme.textColor.b, 0.7)
            horizontalAlignment: Text.AlignRight
        }
    }

    // Entire row click handler - but avoid clicking on the favorite icon
    MouseArea {
        anchors.fill: parent
        anchors.leftMargin: 50  // Don't overlap with favorite icon
        hoverEnabled: true
        onClicked: root.stationClicked(root.index, root.tags)  // CHANGED: Pass tags
    }

    Component.onCompleted: {
        // Debug log for each delegate row
        console.log("[RadioStationItem] idx:", root.index,
                    "name:", root.name,
                    "url:", root.url,
                    "tags:", root.tags,
                    "bitrate:", root.bitrate,
                    "fav:", root.isFavorite)
    }
}
