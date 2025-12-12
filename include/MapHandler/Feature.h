#pragma once
#include "Geometry.h"
#include <QObject>
#include <QVariantMap>
#include "Styler.h"

class AbstractFeature : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Geometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(Styler* style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(int geometryType READ geometryType NOTIFY geometryChanged)

public:
    explicit AbstractFeature(const QString& name = {}, Geometry* geometry = nullptr, QObject* parent = nullptr);
    ~AbstractFeature() override;

    QString name() const { return m_name; }
    void setName(const QString& name);

    Geometry* geometry() const { return m_geometry; }
    void setGeometry(Geometry* g);

    Styler* style() const { return m_style; }
    void setStyle(Styler* s);  

    bool visible() const { return m_visible; }
    void setVisible(bool v);

    int geometryType() const { return m_geometry ? static_cast<int>(m_geometry->geometryType()) : -1; }

signals:
    void nameChanged(const QString& name);
    void geometryChanged();
    void styleChanged();
    void visibleChanged();
    void removed();

private:
    QString m_name;
    Geometry* m_geometry = nullptr;
    Styler* m_style = nullptr;  
    bool m_visible = true;
};


template <typename GeometryT>
class Feature : public AbstractFeature
{
public:
    explicit Feature(const QString& name = {}, GeometryT* geometry = nullptr, QObject* parent = nullptr)
        : AbstractFeature(name, geometry, parent) {}

    GeometryT* geometry() const {
        return static_cast<GeometryT*>(AbstractFeature::geometry());
    }

    void setGeometry(GeometryT* g) {
        AbstractFeature::setGeometry(g);
    }

    using StyleType = typename std::conditional<std::is_same<GeometryT, Point>::value, PointStyle,
                            typename std::conditional<std::is_same<GeometryT, LineString>::value || std::is_same<GeometryT, MultiLineString>::value, LineStyle,
                            typename std::conditional<std::is_same<GeometryT, Polygon>::value || std::is_same<GeometryT, MultiPolygon>::value, PolygonStyle,
                            typename std::conditional<std::is_same<GeometryT, Circle>::value, CircleStyle,
                            Styler>::type>::type>::type>::type;

    StyleType* style() const {
        return static_cast<StyleType*>(AbstractFeature::style());
    }

    void setStyle(StyleType* s) {
        AbstractFeature::setStyle(s);
    }
};

using PointFeature        = Feature<Point>;
using CircleFeature       = Feature<Circle>;
using LineFeature         = Feature<LineString>;
using PolygonFeature      = Feature<Polygon>;
using MultiLineFeature    = Feature<MultiLineString>;
using MultiPolygonFeature = Feature<MultiPolygon>;