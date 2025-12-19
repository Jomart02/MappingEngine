// resources/qml/Map.qml
import QtQuick 2.15
import QtLocation 6.5
import QtPositioning 6.5
import com.mycompany.map 1.0

Item {
    id: root
    anchors.fill: parent
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

        Behavior on bearing { NumberAnimation { duration: 600; easing.type: Easing.OutCubic } }

        property var __items: ({})

        // === Масштабная линейка (Scale Bar) ===
        // Внутри Map (добавьте этот Item вместо предыдущей масштабной линейки)
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

            // Отступы сверху и снизу
            property int topOffset: 60
            property int bottomOffset: 60

            // Вычисляемая высота для сегментов
            readonly property real usableHeight: Math.max(0, height - topOffset - bottomOffset)

            // Данные шкалы (обновляются из updateScaleBar)
            property var scaleSegments: [] // [{y, height, color, label}]

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

                // Подписи: верх, середина, низ
                const labels = [
                    { y: topPixelY, text: niceDistance + " M" },
                    { y: topPixelY + usableHeight / 2, text: (niceDistance / 2).toString() + " M" },
                    { y: bottomPixelY, text: "0 M" }
                ]

                // Передаём данные в модель
                scaleSegmentsModel.clear()
                for (let i = 0; i < segments.length; i++) {
                    scaleSegmentsModel.append(segments[i])
                }
                // Обновляем подписи отдельно
                topLabel.text = niceDistance + " M"
                middleLabel.text = (niceDistance / 2).toString() + " M"
                bottomLabel.text = "0 M"

                topLabel.y = topPixelY
                middleLabel.y = topPixelY + usableHeight / 2
                bottomLabel.y = bottomPixelY
            }

            // Модель для Repeater
            ListModel {
                id: scaleSegmentsModel
            }

            // Фоновые сегменты
            Repeater {
                model: scaleSegmentsModel
                Rectangle {
                    x: 0
                    y: model.y
                    width: 8
                    height: model.height
                    color: model.color
                }
            }

            // Текстовые метки
            Text {
                id: topLabel
                x: 16
                color: textColor
                font.pixelSize: 20
                font.bold: true
                font.family: "Arial"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: middleLabel
                x: 16
                color: textColor
                font.pixelSize: 20
                font.bold: true
                font.family: "Arial"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: bottomLabel
                x: 16
                color: textColor
                font.pixelSize: 20
                font.bold: true
                font.family: "Arial"
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }

            // Подписываемся на изменения карты
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

    function createOrUpdateItem(name, geometryType, geometryData, style) {
        var item = map.__items[name];

        // Если элемента ещё нет — создаём нужный тип
        if (!item) {
            if (geometryType === GeometryType.Point) {
                item = pointComponent.createObject(map);
            }
            else if (geometryType === GeometryType.LineString || geometryType === GeometryType.MultiLineString) {
                item = polylineComponent.createObject(map);
            }
            else if (geometryType === GeometryType.Polygon || geometryType === GeometryType.MultiPolygon) {
                item = polygonComponent.createObject(map);
            }
            else if (geometryType === GeometryType.Circle) {
                item = circleComponent.createObject(map);
            }
            else {
                console.warn("Map.qml: Unsupported geometry type", geometryType);
                return;
            }

            // Сохраняем и добавляем на карту
            map.__items[name] = item;
            map.addMapItem(item);
        }

        // === Общие свойства ===
        item.visible = style.visible !== undefined ? style.visible : true;
        item.z = style.z !== undefined ? style.z : 10;

        // === ТОЧКА ===
        if (geometryType === GeometryType.Point) {
            item.coordinate = QtPositioning.coordinate(geometryData.lat, geometryData.lon);

            // Поддержка кастомной иконки
            if (style.icon && item.sourceItem) {
                item.sourceItem.source = style.icon;
                if (style.iconScale !== undefined) {
                    item.sourceItem.scale = style.iconScale;
                }
            }
        }

        // === ЛИНИЯ (LineString / MultiLineString) ===
        else if (geometryType === GeometryType.LineString || geometryType === GeometryType.MultiLineString) {
            var path = [];
            var coords = geometryData.coordinates;  // это QVariantList из QVariantMap{{lat,lon}}

            for (var i = 0; i < coords.length; ++i) {
                var c = coords[i];
                path.push(QtPositioning.coordinate(c.lat, c.lon));
            }
            item.path = path;

            item.line.width = style.weight !== undefined ? style.weight : 6;
            item.line.color = style.color || "#e63946";
            if (style.opacity !== undefined) item.opacity = style.opacity;
        }

        // === ПОЛИГОН ===
        else if (geometryType === GeometryType.Polygon || geometryType === GeometryType.MultiPolygon) {
            var exterior = [];
            var rings = geometryData.exterior;  // мы передаём exteriorRing → exterior

            for (var j = 0; j < rings.length; ++j) {
                var p = rings[j];
                exterior.push(QtPositioning.coordinate(p.lat, p.lon));
            }
            item.path = exterior;

            item.color = style.fillColor || "#457b9d44";
            item.border.width = style.weight !== undefined ? style.weight : 4;
            item.border.color = style.color || "#457b9d";
            if (style.opacity !== undefined) item.opacity = style.opacity;
        }

        // === КРУГ ===
        else if (geometryType === GeometryType.Circle) {
            item.center = QtPositioning.coordinate(geometryData.lat, geometryData.lon);
            item.radius = geometryData.radius;  // в метрах

            item.color = style.fillColor || "#ff006644";
            item.border.width = style.weight !== undefined ? style.weight : 5;
            item.border.color = style.color || "#ff0066";
            if (style.opacity !== undefined) item.opacity = style.opacity;
        }
    }

    // ПОДКЛЮЧЕНИЕ К C++
    Connections {
        target: MapHandler
        function onFeatureAdded(name, type, geometry, style) {
            console.log("Update")
            root.createOrUpdateItem(name, type, geometry, style)
        }
        function onFeatureUpdated(name, type, geometry, style) {
            root.createOrUpdateItem(name, type, geometry, style)
        }
        function onFeatureRemoved(name) {
            if (map.__items[name]) {
                map.removeMapItem(map.__items[name])
                map.__items[name].destroy()
                delete map.__items[name]
            }
        }
    }

    // КОМПОНЕНТЫ — ОНИ ДОЛЖНЫ БЫТЬ НА УРОВНЕ root
    Component { id: pointComponent
        MapQuickItem {
            anchorPoint.x: image.width / 2
            anchorPoint.y: image.height
            sourceItem: Image {
                id: image
                width: 48; height: 48
                // source: "qrc:/icons/marker.png"
                fillMode: Image.PreserveAspectFit
            }
        }
    }

    Component { id: polylineComponent
        MapPolyline {
            line.width: 6
            line.color: "#e63946"
        }
    }

    Component { id: polygonComponent
        MapPolygon {
            color: "#457b9d44"
            border.width: 4
            border.color: "#457b9d"
        }
    }

    Component { id: circleComponent
        MapCircle {
            color: "#ff006644"
            border.width: 5
            border.color: "#ff0066"
        }
    }

    // Клик и зум
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true // Включаем отслеживание наведения для координат
        NumberAnimation {
            id: zoomAnimation
            target: map
            property: "zoomLevel"
            duration: 200
            easing.type: Easing.InOutQuad
        }


        property var dragStartPoint: undefined // Точка начала перетаскивания
        property bool isDragging: false

        onPressed: function(mouse) {
            dragStartPoint = Qt.point(mouse.x, mouse.y)
            isDragging = false
            console.log("Clicked at latitude:")
        }

        onPositionChanged: function(mouse) {
            if (mouse.buttons & Qt.LeftButton && dragStartPoint) {
                var deltaX = mouse.x - dragStartPoint.x
                var deltaY = mouse.y - dragStartPoint.y

                if (deltaX > 5 || deltaY > 5) {
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
            mouse.accepted = true; // остановить дальнейшую обработку
            var clickCoordinate = map.toCoordinate(Qt.point(mouse.x, mouse.y))
            console.log("Clicked at latitude:", clickCoordinate.latitude, "longitude:", clickCoordinate.longitude)
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


    // Rectangle {
    //     anchors.top: parent.top; anchors.right: parent.right; anchors.margins: 20
    //     width: 56; height: 56; radius: 28
    //     color: "#AA000000"; border.color: "white"; border.width: 2
    //     rotation: -map.bearing
    //     Text { anchors.centerIn: parent; text: "Reset"; color: "white"; font.pixelSize: 20; font.bold: true }
    //     MouseArea { anchors.fill: parent; onClicked: map.bearing = 0 }
    // }
}
