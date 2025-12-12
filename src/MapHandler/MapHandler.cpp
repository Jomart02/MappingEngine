#include "MapHandler.h"
#include "Feature.h"

MapHandler* MapHandler::instance()
{
    static MapHandler inst;
    return &inst;
}

MapHandler::MapHandler(QObject* parent) : QObject(parent) {}

void MapHandler::setMapItem(QQuickItem* item)
{
    if (m_mapItem == item) return;
    m_mapItem = item;
    emit mapItemChanged();
}

void MapHandler::addFeature(AbstractFeature* feature)
{
    if (!feature || feature->name().isEmpty()) {
        qWarning() << "Feature must have a non-empty name";
        return;
    }

    const QString name = feature->name();

    if (m_features.contains(name)) {
        qWarning() << "Feature" << name << "already exists → replacing";
        removeFeature(name);
    }

    feature->setParent(this);
    m_features[name] = feature;

    QVariantMap geomData;
    Geometry* g = feature->geometry();

    switch (g->geometryType()) {
    case GeometryType::Point: {
        auto* p = static_cast<Point*>(g);
        geomData["type"] = "Point";
        geomData["lat"] = p->coordinate.lat;
        geomData["lon"] = p->coordinate.lon;
        break;
    }
    case GeometryType::LineString: {
        auto* ls = static_cast<LineString*>(g);
        geomData["type"] = "LineString";
        QVariantList coords;
        for (const auto& c : ls->coordinates)
            coords << QVariantMap{{"lat", c.lat}, {"lon", c.lon}};
        geomData["coordinates"] = coords;
        break;
    }
    case GeometryType::Polygon: {
        auto* poly = static_cast<Polygon*>(g);
        geomData["type"] = "Polygon";
        QVariantList exterior;
        for (const auto& c : poly->exteriorRing)
            exterior << QVariantMap{{"lat", c.lat}, {"lon", c.lon}};
        geomData["exterior"] = exterior;
        break;
    }
    case GeometryType::Circle: {
        auto* c = static_cast<Circle*>(g);
        geomData["type"] = "Circle";
        geomData["lat"] = c->center.lat;
        geomData["lon"] = c->center.lon;
        geomData["radius"] = c->radiusMeters;
        break;
        break;
    }
    }
    
    emit featureAdded(name, feature->geometryType(), geomData, feature->style()->toVariantMap());
    setupConnections(feature);
}

void MapHandler::setupConnections(AbstractFeature* f)
{
    const QString originalName = f->name();

    // Вспомогательная функция — конвертирует Geometry → QVariantMap
    auto toVariantMap = [f]() -> QVariantMap {
        QVariantMap map;
        Geometry* g = f->geometry();
        if (!g) return map;

        switch (g->geometryType()) {
        case GeometryType::Point: {
            auto* p = static_cast<Point*>(g);
            map["type"] = "Point";
            map["lat"] = p->coordinate.lat;
            map["lon"] = p->coordinate.lon;
            break;
        }
        case GeometryType::LineString:
        case GeometryType::MultiLineString: {
            auto* ls = static_cast<LineString*>(g);
            map["type"] = "LineString";
            QVariantList coords;
            for (const auto& c : ls->coordinates) {
                coords.append(QVariantMap{{"lat", c.lat}, {"lon", c.lon}});
            }
            map["coordinates"] = coords;
            break;
        }
        case GeometryType::Polygon:
        case GeometryType::MultiPolygon: {
            auto* poly = static_cast<Polygon*>(g);
            map["type"] = "Polygon";
            QVariantList exterior;
            for (const auto& c : poly->exteriorRing) {
                exterior.append(QVariantMap{{"lat", c.lat}, {"lon", c.lon}});
            }
            map["exterior"] = exterior;
            break;
        }
        case GeometryType::Circle: {
            auto* c = static_cast<Circle*>(g);
            map["type"] = "Circle";
            map["lat"] = c->center.lat;
            map["lon"] = c->center.lon;
            map["radius"] = c->radiusMeters;
            break;
        }
        default:
            break;
        }
        return map;
    };

    auto updateFeature = [this, f, toVariantMap, originalName]() {
        QString currentName = f->name().isEmpty() ? originalName : f->name();
        QVariantMap geomData = toVariantMap();
        QVariantMap style = f->style()->toVariantMap();

        style["visible"] = f->visible();
        emit featureUpdated(currentName, geomData, style);
    };

    connect(f, &AbstractFeature::geometryChanged, this, updateFeature);
    connect(f, &AbstractFeature::styleChanged,    this, updateFeature);
    connect(f, &AbstractFeature::visibleChanged,   this, updateFeature);

    connect(f, &AbstractFeature::nameChanged, this, [this, f, toVariantMap](const QString& newName) {
        if (newName.isEmpty() || newName == f->name()) return;

        QString oldName = f->name(); 
        if (m_features.contains(oldName)) {
            m_features.remove(oldName);
            m_features[newName] = f;

            QVariantMap geomData = toVariantMap();
            QVariantMap style = f->style()->toVariantMap();
            style["visible"] = f->visible();

            emit featureUpdated(newName, geomData, style);
        }
    });


}

void MapHandler::removeFeature(const QString& name) {
    if (AbstractFeature* f = m_features.take(name)) {
        emit featureRemoved(name);
        f->deleteLater();
    }
}

void MapHandler::removeFeature(AbstractFeature* f) {
    if (f) removeFeature(f->name());
}

void MapHandler::clearAll() {
    for (const QString& n : m_features.keys()) emit featureRemoved(n);
    qDeleteAll(m_features);
    m_features.clear();
}

AbstractFeature* MapHandler::feature(const QString& name) const {
    return m_features.value(name);
}

void MapHandler::setVisible(const QString& name, bool v) {
    if (auto* f = feature(name)) f->setVisible(v);
}

void MapHandler::centerOn(const QString& name) {

}