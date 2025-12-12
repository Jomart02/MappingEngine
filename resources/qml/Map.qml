// resources/qml/Map.qml
import QtQuick 2.15
import QtLocation 6.5
import QtPositioning 6.5

Item {
    id: root
    anchors.fill: parent

    signal mapClicked(double latitude, double longitude)

    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter { name: "osm.mapping.highdpi_tiles"; value: true }
    }

    Map {
        id: mapItem
        objectName: "mapItem"
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(55.7558, 37.6173)
        zoomLevel: 12
        minimumZoomLevel: 2
        maximumZoomLevel: 20

        // Плавный поворот карты
        Behavior on bearing {
            NumberAnimation { duration: 600; easing.type: Easing.OutCubic }
        }

        property var markers: []

        function addMarker(lat, lon, title = "", icon = "") {
            var path = icon ? "qrc:/qml/" + icon + ".qml" : "qrc:/qml/Marker.qml"
            var comp = Qt.createComponent(path)
            if (comp.status !== Component.Ready) { console.error(comp.errorString()); return -1 }
            var m = comp.createObject(mapItem, { coordinate: QtPositioning.coordinate(lat, lon) })
            if (!m) return -1
            m.title = title
            m.z = 1000
            mapItem.addMapItem(m)
            markers.push(m)
            return markers.length - 1
        }

        function clearMarkers() { while (markers.length) { var m = markers.pop(); mapItem.removeMapItem(m); m.destroy() } }

        // ЕДИНСТВЕННЫЙ MouseArea — всё делает
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            hoverEnabled: true

            property point startPos
            property bool dragging: false

            onPressed: (mouse) => {
                if (!(mouse.modifiers & Qt.ShiftModifier)) {
                    startPos = Qt.point(mouse.x, mouse.y)
                    dragging = false
                }
            }

            onPositionChanged: (mouse) => {
                if (mouse.buttons & Qt.LeftButton && !(mouse.modifiers & Qt.ShiftModifier)) {
                    var dx = mouse.x - startPos.x
                    var dy = mouse.y - startPos.y
                    if (!dragging && (Math.abs(dx) > 8 || Math.abs(dy) > 8)) dragging = true
                    if (dragging) {
                        var c = Qt.point(mapItem.width/2, mapItem.height/2)
                        mapItem.center = mapItem.toCoordinate(Qt.point(c.x - dx, c.y - dy))
                        startPos = Qt.point(mouse.x, mouse.y)
                    }
                }
            }

            onReleased: (mouse) => {
                if (!dragging && !(mouse.modifiers & Qt.ShiftModifier)) {
                    var coord = mapItem.toCoordinate(Qt.point(mouse.x, mouse.y))
                    root.mapClicked(coord.latitude, coord.longitude)
                }
                dragging = false
            }

            onWheel: (wheel) => {
                if (wheel.modifiers & Qt.ShiftModifier) {
                    // Shift + колёсико — поворот
                    mapItem.bearing += wheel.angleDelta.y > 0 ? 15 : -15
                } else {
                    // Обычный зум к курсору
                    var delta = wheel.angleDelta.y > 0 ? 1 : -1
                    var newZoom = Math.max(2, Math.min(20, mapItem.zoomLevel + delta))
                    var coord = mapItem.toCoordinate(Qt.point(wheel.x, wheel.y))
                    mapItem.zoomLevel = newZoom
                    mapItem.center = coord
                }
                wheel.accepted = true
            }
        }
    }

    // ПОДСКАЗКА — ВСЕГДА ГОРИЗОНТАЛЬНАЯ!
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 20
        width: hint.implicitWidth + 40
        height: hint.implicitHeight + 24
        radius: 14
        color: "#AA000000"
        border.color: "white"
        border.width: 2

        rotation: -mapItem.bearing   // КЛЮЧЕВАЯ СТРОКА!

        Text {
            id: hint
            anchors.centerIn: parent
            text: "Shift + колёсико — повернуть карту"
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }
    }

    // КНОПКА СБРОСА — ТОЖЕ ВСЕГДА ГОРИЗОНТАЛЬНАЯ!
    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 20
        width: 56; height: 56
        radius: 28
        color: "#AA000000"
        border.color: "white"
        border.width: 2

        rotation: -mapItem.bearing   // ВСЕГДА ЧИТАЕМАЯ!

        Text {
            anchors.centerIn: parent
            text: "Reset"
            color: "white"
            font.pixelSize: 28
            font.bold: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mapItem.bearing = 0
        }
    }
}