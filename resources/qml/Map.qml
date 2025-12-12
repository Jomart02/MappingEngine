// resources/qml/Map.qml
import QtQuick 2.15
import QtLocation 6.5
import QtPositioning 6.5
import com.mycompany.map 1.0

Item {
    id: root
    anchors.fill: parent
    signal mapClicked(double latitude, double longitude)

    // ФИКСИМ ОШИБКУ OSM
    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: "true" }
        PluginParameter { name: "osm.mapping.highdpi_tiles"; value: true }
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

        // ВАЖНО: ФУНКЦИЯ НА УРОВНЕ root, НЕ ВНУТРИ Map!
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
        function onFeatureUpdated(name, geometry, style) {
            root.createOrUpdateItem(name, -1, geometry, style)
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
        anchors.fill: parent
        onClicked: (mouse) => {
            if (mouse.modifiers & Qt.ShiftModifier) return
            var c = map.toCoordinate(Qt.point(mouse.x, mouse.y))
            root.mapClicked(c.latitude, c.longitude)
        }
        onWheel: (wheel) => {
            if (wheel.modifiers & Qt.ShiftModifier) {
                map.bearing += wheel.angleDelta.y > 0 ? 15 : -15
            } else {
                var factor = wheel.angleDelta.y > 0 ? 1.2 : 0.83
                var coord = map.toCoordinate(Qt.point(wheel.x, wheel.y))
                map.zoomLevel *= factor
                map.center = coord
            }
            wheel.accepted = true
        }
    }

    // Подсказки
    Rectangle {
        anchors.bottom: parent.bottom; anchors.right: parent.right; anchors.margins: 20
        width: hintText.implicitWidth + 40; height: 48; radius: 14
        color: "#AA000000"; border.color: "white"; border.width: 2
        rotation: -map.bearing
        Text { id: hintText; anchors.centerIn: parent; text: "Shift + колёсико — повернуть"; color: "white"; font.pixelSize: 16; font.bold: true }
    }

    Rectangle {
        anchors.top: parent.top; anchors.right: parent.right; anchors.margins: 20
        width: 56; height: 56; radius: 28
        color: "#AA000000"; border.color: "white"; border.width: 2
        rotation: -map.bearing
        Text { anchors.centerIn: parent; text: "Reset"; color: "white"; font.pixelSize: 20; font.bold: true }
        MouseArea { anchors.fill: parent; onClicked: map.bearing = 0 }
    }
}