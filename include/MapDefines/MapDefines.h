#pragma once
#include <QMetaType>
#include <QDebug>

struct Coordinate
{
    double lon = 0.0;
    double lat = 0.0;

    Coordinate() = default;
    Coordinate(double longitude, double latitude) : lon(longitude), lat(latitude) {}

    bool operator==(const Coordinate& other) const {
        return qFuzzyCompare(lon, other.lon) && qFuzzyCompare(lat, other.lat);
    }
};

Q_DECLARE_METATYPE(Coordinate)