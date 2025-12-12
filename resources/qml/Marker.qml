// resources/qml/Marker.qml
import QtQuick 2.15
import QtPositioning 6.5          // Qt 6
import QtLocation 6.5

MapQuickItem {
    id: root

    zoomLevel: 0
    anchorPoint.x: markerItem.width / 2
    anchorPoint.y: markerItem.height + 10

    property string title: "•"
    property bool selected: false

    // Прыжок при добавлении (работает в Qt 6!)
    Behavior on coordinate {
        CoordinateAnimation {
            duration: 900
            easing.type: Easing.OutBounce
        }
    }

    // ← ВСЁ ВНУТРИ sourceItem — это обязательное требование Qt 6!
    sourceItem: Item {
        id: markerItem
        width: bubble.width
        height: bubble.height

        // === Основной маркер ===
        Rectangle {
            id: bubble
            width: Math.max(44, label.implicitWidth + 32)
            height: 44
            radius: 22
            color: root.selected ? "#00A86B" : "#E03E3E"
            border.color: "white"
            border.width: 4
            antialiasing: true

            // Тень
            Rectangle {
                anchors.fill: parent
                radius: parent.radius
                color: "black"
                opacity: 0.3
                y: 5
                z: -1
            }

            Text {
                id: label
                anchors.centerIn: parent
                text: root.title
                color: "white"
                font.pixelSize: 15
                font.bold: true
            }

            // Плавное появление
            opacity: 0
            scale: 0.2

            SequentialAnimation {
                running: true
                ParallelAnimation {
                    NumberAnimation { target: bubble; property: "opacity"; to: 1.0; duration: 500; easing.type: Easing.OutCubic }
                    NumberAnimation { target: bubble; property: "scale";    to: 1.0; duration: 900; easing.type: Easing.OutBack }
                }
            }

            // Пульсация при наведении
            NumberAnimation on scale {
                id: pulseAnim
                from: 1.0; to: 1.4; duration: 700
                loops: Animation.Infinite
                running: false
                easing.type: Easing.InOutQuad
            }
        }

        // Подсветка при выделении
        Rectangle {
            anchors.centerIn: bubble
            width: bubble.width + 30
            height: bubble.height + 30
            radius: width / 2
            color: "white"
            opacity: root.selected ? 0.5 : 0
            z: -1
            Behavior on opacity { NumberAnimation { duration: 300 } }
        }

        // MouseArea — теперь внутри sourceItem, всё ок!
        MouseArea {
            anchors.fill: bubble
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onEntered: pulseAnim.start()
            onExited: {
                pulseAnim.stop()
                bubble.scale = 1.0
            }
            onClicked: root.selected = !root.selected
        }
    }
}