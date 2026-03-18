#include "MapHandler.h"
#include "Feature.h"
#include "MapPluginEngine.h"
#include "MapTileFetcher.h"
#include <QTimer>
MapHandler* MapHandler::instance()
{
    static MapHandler inst;
    return &inst;
}

MapHandler::MapHandler(QObject* parent) : QObject(parent) {

    // QTimer::singleShot(5000, [=](){
    //     qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAA"<<MapPluginEngine::instance();
    //     QTimer::singleShot(10000,[=](){
    //         MapPluginEngine::instance()->getFetcher()->setOverlay(false);  
    //         qDebug() << "bbbbbbbbbbbb";
    //         emit requestMapRefresh();
    //         QTimer::singleShot(10000,[=](){
    //             MapPluginEngine::instance()->getFetcher()->setOverlay(true);  
    //             qDebug() << "33333333333333333333333333333333";
    //             emit requestMapRefresh();
    //         });
    //     });
    // });
    //  qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAA"<<MapPluginEngine::instance();
}

void MapHandler::addFeature(AbstractFeature* feature)
{
    if (!feature || feature->name().isEmpty()) return;
    
    QString name = feature->name();
    if (m_features.contains(name)) removeFeature(name);

    m_features[name] = feature;
    feature->setParent(this);

    QVariantMap geomData = geometryToVariant(feature->geometry());
    QVariantMap styleData = feature->style()->toVariantMap();
    styleData["visible"] = feature->visible();

    emit featureAdded(name, (int)feature->geometry()->geometryType(), geomData, styleData);
    setupConnections(feature);
}


void MapHandler::addGroup(FeatureGroup* group) {
    if(!group) return;
    if(m_groups.contains(group->name())) removeGroup(group->name());

    m_groups[group->name()] = group;
    group->setParent(this);

    for(auto* f : group->features()) {
        addFeature(f); 
    }
    
    connect(group, &FeatureGroup::featureAddedToGroup, this, [this](AbstractFeature* f){
        this->addFeature(f); 
    });

    connect(group, &FeatureGroup::featureRemovedFromGroup, this, [this](AbstractFeature* f){
        this->removeFeature(f->name());
    });
    
    for(auto* f : group->features()) {
        addFeature(f);
    }
}

void MapHandler::removeGroup(const QString& groupName) {
    if(FeatureGroup* g = m_groups.take(groupName)) {
        // Remove all its features from QML
        for(auto* f : g->features()) {
            removeFeature(f->name());
        }
        g->deleteLater();
    }
}

FeatureGroup* MapHandler::getGroup(const QString& name) const {
    return m_groups.value(name);
}

void MapHandler::setupConnections(AbstractFeature* f)
{
    connect(f, &AbstractFeature::geometryChanged, this, &MapHandler::onFeatureChanged);
    connect(f, &AbstractFeature::styleChanged, this, &MapHandler::onFeatureChanged);
    connect(f, &AbstractFeature::visibleChanged, this, &MapHandler::onFeatureChanged);
}


void MapHandler::onFeatureChanged()
{
    AbstractFeature* f = qobject_cast<AbstractFeature*>(sender());
    if (!f || !m_features.contains(f->name())) return;  // Проверка валидности
    
    QVariantMap geom = geometryToVariant(f->geometry());
    QVariantMap style = f->style()->toVariantMap();
    style["visible"] = f->visible();
    emit featureUpdated(f->name(), (int)f->geometry()->geometryType(), geom, style);
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
    for(auto k : m_features.keys()) emit featureRemoved(k);
    m_features.clear();
    m_groups.clear();
}

AbstractFeature* MapHandler::feature(const QString& name) const {
    return m_features.value(name);
}

void MapHandler::setVisible(const QString& name, bool v) {
    if(auto* f = feature(name)) f->setVisible(v);
    if(auto* g = getGroup(name)) g->setVisible(v);
}

void MapHandler::centerOn(const QString& name) {

}

QVariantMap MapHandler::geometryToVariant(Geometry* g) const{
    QVariantMap map;
    if (!g) return map;

    switch (g->geometryType()) {
        case GeometryType::Point: {
            auto* p = static_cast<Point*>(g);
            map["type"] = "Point";
            map["lat"] = p->coordinate.lat;
            map["lon"] = p->coordinate.lon;
            break;
        }
        case GeometryType::LineString: {
            auto* ls = static_cast<LineString*>(g);
            map["type"] = "LineString";
            QVariantList coords;
            for (const auto& c : ls->coordinates) coords << QVariantMap{{"lat", c.lat}, {"lon", c.lon}};
            map["coordinates"] = coords;
            break;
        }
        case GeometryType::Polygon: {
            auto* poly = static_cast<Polygon*>(g);
            map["type"] = "Polygon";
            QVariantList exterior;
            for (const auto& c : poly->exteriorRing) exterior << QVariantMap{{"lat", c.lat}, {"lon", c.lon}};
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
        default: break;
    }
    return map;
}