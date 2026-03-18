import QtQuick 2.15
import QtLocation 6.5
import QtPositioning 6.5
import com.mycompany.map 1.0

Item {
    id: featureManager

    // Свойство, в которое мы передадим саму карту извне
    required property Map targetMap

    // Внутреннее хранилище объектов
    property var __items: ({})

    // === ЛОГИКА ===
    function createOrUpdateItem(name, geometryType, geometryData, style) {
        if (!targetMap) return;

        var item = featureManager.__items[name];

        // 1. Создание (если нет)
        if (!item) {
            if (geometryType === GeometryType.Point) {
                item = pointComponent.createObject(targetMap);
            }
            else if (geometryType === GeometryType.LineString || geometryType === GeometryType.MultiLineString) {
                item = polylineComponent.createObject(targetMap);
            }
            else if (geometryType === GeometryType.Polygon || geometryType === GeometryType.MultiPolygon) {
                item = polygonComponent.createObject(targetMap);
            }
            else if (geometryType === GeometryType.Circle) {
                item = circleComponent.createObject(targetMap);
            }
            else {
                console.warn("FeatureManager: Unsupported geometry type", geometryType);
                return;
            }

            featureManager.__items[name] = item;
            targetMap.addMapItem(item);
        }

        // 2. Обновление свойств (переиспользование кода)
        updateItemProperties(item, geometryType, geometryData, style);
    }

    function updateItemProperties(item, geometryType, geometryData, style) {
        // Общие свойства
        item.visible = style.visible !== undefined ? style.visible : true;
        item.z = style.z !== undefined ? style.z : 10;

        // Точка
        if (geometryType === GeometryType.Point) {
            item.coordinate = QtPositioning.coordinate(geometryData.lat, geometryData.lon);
            if (style.icon && item.sourceItem) {
                item.sourceItem.source = style.icon;
                if (style.iconScale !== undefined) item.sourceItem.scale = style.iconScale;
            }
        }
        // Линия
        else if (geometryType === GeometryType.LineString || geometryType === GeometryType.MultiLineString) {
            var path = [];
            var coords = geometryData.coordinates;
            for (var i = 0; i < coords.length; ++i) {
                path.push(QtPositioning.coordinate(coords[i].lat, coords[i].lon));
            }
            item.path = path;
            item.line.width = style.weight !== undefined ? style.weight : 6;
            item.line.color = style.color || "#e63946";
            if (style.opacity !== undefined) item.opacity = style.opacity;
            console.log(style.dashArray)
            // Сохраняем dash pattern для кастомного рендеринга
            if (style.dashArray !== undefined && style.dashArray !== "") {
                item.dashPattern = style.dashArray.split(",").map(Number);
            } else {
                item.dashPattern = [];
            }
        }
        // Полигон
        else if (geometryType === GeometryType.Polygon || geometryType === GeometryType.MultiPolygon) {
            var exterior = [];
            var rings = geometryData.exterior;
            for (var j = 0; j < rings.length; ++j) {
                exterior.push(QtPositioning.coordinate(rings[j].lat, rings[j].lon));
            }
            item.path = exterior;
            item.color = style.fillColor || "#457b9d44";
            item.border.width = style.weight !== undefined ? style.weight : 4;
            item.border.color = style.color || "#457b9d";
            if (style.opacity !== undefined) item.opacity = style.opacity;
        }
        // Круг
        else if (geometryType === GeometryType.Circle) {
            item.center = QtPositioning.coordinate(geometryData.lat, geometryData.lon);
            item.radius = geometryData.radius;
            item.color = style.fillColor || "#ff006644";
            item.border.width = style.weight !== undefined ? style.weight : 5;
            item.border.color = style.color || "#ff0066";
            if (style.opacity !== undefined) item.opacity = style.opacity;
        }
    }

    // === СВЯЗЬ С C++ ===
    Connections {
        target: MapHandler
        function onFeatureAdded(name, type, geometry, style) {
            featureManager.createOrUpdateItem(name, type, geometry, style)
        }
        function onFeatureUpdated(name, type, geometry, style) {
            featureManager.createOrUpdateItem(name, type, geometry, style)
        }
        function onFeatureRemoved(name) {
            var item = featureManager.__items[name];
            if (item) {
                targetMap.removeMapItem(item)
                item.destroy()
                delete featureManager.__items[name]
            }
        }
        function onRequestMapRefresh() {
            targetMap.clearData()
            // Возможно нужно очистить и локальный js-объект __items, если clearData удаляет всё визуально
            featureManager.__items = ({})
        }
    }

    // === ШАБЛОНЫ КОМПОНЕНТОВ ===
    Component { id: pointComponent
        MapQuickItem {
            anchorPoint.x: image.width / 2
            anchorPoint.y: image.height
            sourceItem: Image { id: image; width: 48; height: 48; fillMode: Image.PreserveAspectFit }
        }
    }
    Component { id: polylineComponent; MapPolyline { line.width: 6; line.color: "#e63946" } }
    Component { id: polygonComponent; MapPolygon { color: "#457b9d44"; border.width: 4; border.color: "#457b9d" } }
    Component { id: circleComponent; MapCircle { color: "#ff006644"; border.width: 5; border.color: "#ff0066" } }
}