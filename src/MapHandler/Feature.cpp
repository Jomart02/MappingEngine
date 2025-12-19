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