/*
 * SPDX-FileCopyrightText: 2020 George Florea BÄƒnuÈ™ <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.haruna
import org.kde.haruna.settings

PlaylistItemDelegate {
    id: root
    implicitHeight: 60

    contentItem: Rectangle {
        width: root.width
        height: root.height

        // Match RadioStationItem structure exactly - single background
        color: root.isPlaying 
               ? Qt.rgba(0, 0.83, 1, 0.08)  // Very subtle blue tint when playing
               : (root.isSelected 
                  ? Qt.rgba(1, 1, 1, 0.1)   // Light grey when selected
                  : (root.state === "Hovered" ? Qt.rgba(1, 1, 1, 0.1) : "transparent"))
        border.width: 1
        border.color: root.isPlaying ? "#00D4FF" : Qt.rgba(1, 1, 1, 0.2)
        radius: 4

        Behavior on color {
            ColorAnimation { duration: 150 }
        }
        
        Behavior on border.color {
            ColorAnimation { duration: 150 }
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: Kirigami.Units.smallSpacing
            spacing: Kirigami.Units.smallSpacing

            // Drag handle - vertically centered
            Kirigami.ListItemDragHandle {
                id: dragHandle
                listItem: root.dragRect
                listView: root.ListView?.view
                onMoveRequested: function (oldIndex, newIndex) {
                    root.selectItem(oldIndex, PlaylistFilterProxyModel.Single)
                    root.moveItems(oldIndex, newIndex)
                }
                Layout.preferredWidth: 40
                Layout.preferredHeight: 40
                Layout.alignment: Qt.AlignVCenter

                onDragActiveChanged: {
                    root.cacheItem()
                }
            }

            // Row number - vertically centered
            Label {
                text: root.padRowNumberAsString()
                color: root.getLabelColor()
                visible: PlaylistSettings.showRowNumber
                font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.9
                horizontalAlignment: Qt.AlignCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredWidth: 30
                Layout.fillHeight: true
            }

            Rectangle {
                color: Qt.alpha(Kirigami.Theme.alternateBackgroundColor, 0.6)
                visible: PlaylistSettings.showRowNumber
                Layout.preferredWidth: 1
                Layout.fillHeight: true
            }

            // Playing icon - vertically centered
            Kirigami.Icon {
                source: "media-playback-start"
                color: root.isPlaying ? "#00D4FF" : root.getLabelColor()
                visible: root.isPlaying
                Layout.alignment: Qt.AlignVCenter
                Layout.preferredWidth: Kirigami.Units.iconSizes.small
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
            }

            // Title/Name and Duration column - like RadioStationItem
            ColumnLayout {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                spacing: 2

                // Title/Name
                LabelWithTooltip {
                    color: root.isPlaying ? "#00D4FF" : root.getLabelColor()
                    text: PlaylistSettings.showMediaTitle ? root.title : root.name
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 1.0
                    font.weight: root.isPlaying ? Font.Medium : Font.Normal
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    
                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }

                // Duration below title
                Label {
                    text: root.duration.length > 0 ? root.duration : ""
                    color: Qt.rgba(root.getLabelColor().r,
                                  root.getLabelColor().g,
                                  root.getLabelColor().b, 0.7)
                    font.pointSize: Kirigami.Theme.defaultFont.pointSize * 0.9
                    visible: root.duration.length > 0
                    Layout.fillWidth: true
                }
            }
        }
    }
}
