// resources/qml/Map.qml
import QtQuick 2.15
import QtLocation 6.5
import QtPositioning 6.5
// import com.mycompany.map 1.0 — скорее всего здесь больше не нужен, если используется только внутри FeatureManager

Item {
    id: root
    anchors.fill: parent

    // Сигнал, который слушают внешние элементы (например, Main.qml)
    signal mapClicked(double latitude, double longitude)

    Plugin {
        id: mapPlugin
        name: "MapPlugin"
        // PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: "true" }
        // PluginParameter { name: "osm.mapping.highdpi_tiles"; value: true }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(55.7558, 37.6173)
        zoomLevel: 13
        minimumZoomLevel: 2
        maximumZoomLevel: 20

        Behavior on bearing { 
            NumberAnimation { duration: 600; easing.type: Easing.OutCubic } 
        }

        // =========================================================
        // ПОДКЛЮЧЕНИЕ УПРАВЛЯЮЩЕГО СЛОЯ (ФЬЮЧЕРСЫ/ОБЪЕКТЫ)
        // =========================================================
        // Весь код создания, обновления и удаления объектов ушел сюда.
        // Мы просто передаем id нашей карты (map).
        FeatureManager {
            id: featureManager
            targetMap: map
        }

        // =========================================================
        // МАСШТАБНАЯ ЛИНЕЙКА (Scale Bar) - UI ЭЛЕМЕНТ
        // =========================================================
        Item {
            id: verticalScaleBar
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.leftMargin: 10
            width: 100
            // visible: map.zoomLevel >= 6

            property color evenColor: "#FF9800"
            property color oddColor: "#848484"
            property color textColor: "#FF9800"

            property int topOffset: 60
            property int bottomOffset: 60

            readonly property real usableHeight: Math.max(0, height - topOffset - bottomOffset)
            property var scaleSegments: [] 

            function updateScaleBar() {
                const numSegments = 10
                const offset = topOffset

                if (height <= offset * 2) {
                    scaleSegments = []
                    return
                }

                const topPixelY = offset
                const bottomPixelY = height - offset
                const usableHeight = bottomPixelY - topPixelY

                const topCoord = map.toCoordinate(Qt.point(width / 2, topPixelY))
                const bottomCoord = map.toCoordinate(Qt.point(width / 2, bottomPixelY))
                const distanceMeters = topCoord.distanceTo(bottomCoord)
                const distanceNM = distanceMeters / 1852.0

                let niceDistance = distanceNM
                if (niceDistance < 0.1) niceDistance = Math.floor(niceDistance * 100) / 100
                else if (niceDistance < 1) niceDistance = Math.floor(niceDistance * 10) / 10
                else if (niceDistance < 10) niceDistance = Math.floor(niceDistance)
                else if (niceDistance < 100) niceDistance = Math.floor(niceDistance / 10) * 10
                else if (niceDistance < 1000) niceDistance = Math.floor(niceDistance / 100) * 100
                else niceDistance = Math.floor(niceDistance / 1000) * 1000

                const segmentNM = niceDistance / numSegments

                const segments = []
                for (let i = 0; i < numSegments; i++) {
                    const y = topPixelY + (usableHeight / numSegments) * i
                    const segHeight = usableHeight / numSegments
                    const color = (i % 2 === 0) ? evenColor : oddColor
                    segments.push({
                        y: y,
                        height: segHeight,
                        color: color
                    })
                }

                const labels = [
                    { y: topPixelY, text: niceDistance + " M" },
                    { y: topPixelY + usableHeight / 2, text: (niceDistance / 2).toString() + " M" },
                    { y: bottomPixelY, text: "0 M" }
                ]

                scaleSegmentsModel.clear()
                for (let i = 0; i < segments.length; i++) {
                    scaleSegmentsModel.append(segments[i])
                }
                
                topLabel.text = niceDistance + " M"
                middleLabel.text = (niceDistance / 2).toString() + " M"
                bottomLabel.text = "0 M"

                topLabel.y = topPixelY
                middleLabel.y = topPixelY + usableHeight / 2
                bottomLabel.y = bottomPixelY
            }

            ListModel { id: scaleSegmentsModel }

            Repeater {
                model: scaleSegmentsModel
                Rectangle {
                    x: 0; y: model.y
                    width: 8; height: model.height
                    color: model.color
                }
            }

            Text {
                id: topLabel; x: 16
                color: textColor; font.pixelSize: 20; font.bold: true; font.family: "Arial"
                horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: middleLabel; x: 16
                color: textColor; font.pixelSize: 20; font.bold: true; font.family: "Arial"
                horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter
            }
            Text {
                id: bottomLabel; x: 16
                color: textColor; font.pixelSize: 20; font.bold: true; font.family: "Arial"
                horizontalAlignment: Text.AlignLeft; verticalAlignment: Text.AlignVCenter
            }

            Connections {
                target: map
                function onWidthChanged() { verticalScaleBar.updateScaleBar() }
                function onHeightChanged() { verticalScaleBar.updateScaleBar() }
                function onZoomLevelChanged() { verticalScaleBar.updateScaleBar() }
                function onCenterChanged() { verticalScaleBar.updateScaleBar() }
                function onVisibleRegionChanged() { verticalScaleBar.updateScaleBar() }
            }

            Component.onCompleted: updateScaleBar()
        }
    }

    // =========================================================
    // УПРАВЛЕНИЕ МЫШЬЮ (ZOOM, PAN, CLICK)
    // =========================================================
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true 

        NumberAnimation {
            id: zoomAnimation
            target: map
            property: "zoomLevel"
            duration: 200
            easing.type: Easing.InOutQuad
        }

        property var dragStartPoint: undefined
        property bool isDragging: false

        onPressed: function(mouse) {
            dragStartPoint = Qt.point(mouse.x, mouse.y)
            isDragging = false
            // console.log("Pressed")
        }

        onPositionChanged: function(mouse) {
            if (mouse.buttons & Qt.LeftButton && dragStartPoint) {
                var deltaX = mouse.x - dragStartPoint.x
                var deltaY = mouse.y - dragStartPoint.y

                if (Math.abs(deltaX) > 5 || Math.abs(deltaY) > 5) {
                    isDragging = true 
                }

                var currentCenter = map.toCoordinate(Qt.point(map.width / 2, map.height / 2))
                var newCenterPixel = Qt.point(
                    map.fromCoordinate(currentCenter).x - deltaX,
                    map.fromCoordinate(currentCenter).y - deltaY
                )
                map.center = map.toCoordinate(newCenterPixel)
                dragStartPoint = Qt.point(mouse.x, mouse.y)
            }
        }

        onReleased: function() {
            dragStartPoint = undefined
        }

        onClicked: (mouse) => { 
            if (isDragging) {
                return
            }
            mouse.accepted = true;
            var clickCoordinate = map.toCoordinate(Qt.point(mouse.x, mouse.y))
            console.log("Clicked at latitude:", clickCoordinate.latitude, "longitude:", clickCoordinate.longitude)
            
            // Отправляем сигнал наверх
            root.mapClicked(clickCoordinate.latitude, clickCoordinate.longitude)
        }

        onWheel: (wheel) => {
            if (wheel.modifiers & Qt.ShiftModifier) {
                map.bearing += wheel.angleDelta.y > 0 ? 15 : -15
            } else {
                var targetZoomLevel = wheel.angleDelta.y > 0 ? map.zoomLevel + 1 : map.zoomLevel - 1;
                targetZoomLevel = Math.max(1, Math.min(20, targetZoomLevel));
                zoomAnimation.to = targetZoomLevel;
                zoomAnimation.restart();
            }
            wheel.accepted = true
        }
    }
}