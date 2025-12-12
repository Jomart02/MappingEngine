#pragma once

#include "MapDefines.h"
#include <QVector>
#include <QObject>
#include <QMetaType>


class GeometryType : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Point,
        LineString,
        Polygon,
        Circle,
        MultiPoint,
        MultiLineString,
        MultiPolygon
    };
    Q_ENUM(Type)
};

class Geometry
{
public:
    virtual ~Geometry() = default;
    virtual GeometryType::Type geometryType() const = 0;
};


class Point : public Geometry
{
public:
    Coordinate coordinate;

    explicit Point(const Coordinate& c = {}) : coordinate(c) {}
    GeometryType::Type geometryType() const override { return GeometryType::Point; }
};

class Circle : public Geometry
{
public:
    Coordinate center;
    double radiusMeters = 0.0;

    Circle(const Coordinate& c = {}, double r = 0.0) : center(c), radiusMeters(r) {}
    GeometryType::Type geometryType() const override { return GeometryType::Circle; }
};

class LineString : public Geometry
{
public:
    QVector<Coordinate> coordinates;
    GeometryType::Type geometryType() const override { return GeometryType::LineString; }
};

class Polygon : public Geometry
{
public:
    QVector<Coordinate> exteriorRing;
    QVector<QVector<Coordinate>> interiorRings; // holes

    GeometryType::Type geometryType() const override { return GeometryType::Polygon; }
};

class MultiPoint : public Geometry
{
public:
    QVector<Coordinate> coordinates;
    GeometryType::Type geometryType() const override { return GeometryType::MultiPoint; }
};

class MultiLineString : public Geometry
{
public:
    QVector<QVector<Coordinate>> lines;
    GeometryType::Type geometryType() const override { return GeometryType::MultiLineString; }
};

class MultiPolygon : public Geometry
{
public:
    QVector<Polygon> polygons;
    GeometryType::Type geometryType() const override { return GeometryType::MultiPolygon; }
};