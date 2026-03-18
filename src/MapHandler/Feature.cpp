#include "Feature.h"

AbstractFeature::AbstractFeature(const QString& name, Geometry* geometry, QObject* parent)
    : QObject(parent), m_name(name), m_geometry(geometry)
{

    if (geometry) {
        switch (geometry->geometryType()) {
        case GeometryType::Point:
            m_style = new PointStyle;
            break;
        case GeometryType::LineString:
        case GeometryType::MultiLineString:
            m_style = new LineStyle;
            break;
        case GeometryType::Polygon:
        case GeometryType::MultiPolygon:
            m_style = new PolygonStyle;
            break;
        case GeometryType::Circle:
            m_style = new CircleStyle;
            break;
        default:
            m_style = nullptr;
            break;
        }
    }
}

AbstractFeature::~AbstractFeature()
{
    delete m_geometry;
    delete m_style;
}

void AbstractFeature::setGeometry(Geometry* g)
{
    if (g == m_geometry) return;
    delete m_geometry;
    m_geometry = g;
    emit geometryChanged();
}

void AbstractFeature::setStyle(Styler* s)
{
    if (s == m_style) return;
    delete m_style;
    m_style = s;
    emit styleChanged();
}

void AbstractFeature::setVisible(bool v)
{
    if (m_visible == v) return;
    m_visible = v;
    emit visibleChanged();
}

void AbstractFeature::setName(const QString& name)
{
    if (m_name == name) return;
    m_name = name;
    emit nameChanged(name);
}

void AbstractFeature::notifyStyleChanged() {
    emit styleChanged();
}

void AbstractFeature::notifyGeometryChanged() {
    emit geometryChanged();
}



FeatureGroup::FeatureGroup(const QString& name, QObject* parent)
    : QObject(parent), m_name(name) {}

FeatureGroup::~FeatureGroup() {
    qDeleteAll(m_features);
}

void FeatureGroup::addFeature(AbstractFeature* feature) {
    if (!feature || m_features.contains(feature)) return;
    
    // Feature inherits group's visibility initially
    feature->setVisible(m_visible); 
    feature->setParent(this);
    m_features.append(feature);
    emit featureAddedToGroup(feature);
}

void FeatureGroup::removeFeature(AbstractFeature* feature) {
    if (m_features.removeOne(feature)) {
        emit featureRemovedFromGroup(feature);
        feature->deleteLater();
    }
}

void FeatureGroup::setVisible(bool visible) {
    if (m_visible == visible) return;
    m_visible = visible;
    for(auto* f : m_features) {
        f->setVisible(visible);
    }
    emit groupVisibilityChanged(visible);
}