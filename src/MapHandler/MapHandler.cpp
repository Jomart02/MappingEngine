#include "MapHandler.h"
#include <QQmlProperty>

MapHandler* MapHandler::instance()
{
    static MapHandler* s_instance = new MapHandler();
    return s_instance;
}

MapHandler::MapHandler(QObject *parent)
    : QObject(parent)
{
}

void MapHandler::setMapItem(QQuickItem* item)
{
    if (m_mapItem == item) return;

    m_mapItem = item;
    emit mapItemChanged();
}

void MapHandler::addMarker(double lat, double lon, const QString& title)
{
    if (!m_mapItem) {
        qWarning() << "MapHandler: No map item set!";
        return;
    }

    // ← ИСПРАВЛЕНО: четвёртый параметр — пустая строка (стандартный маркер)
    QVariant returnedValue;
    bool success = QMetaObject::invokeMethod(m_mapItem, "addMarker",
                                             Q_RETURN_ARG(QVariant, returnedValue),
                                             Q_ARG(QVariant, lat),
                                             Q_ARG(QVariant, lon),
                                             Q_ARG(QVariant, title),
                                             Q_ARG(QVariant, QString("")));  // ← четвёртый параметр

    if (!success) {
        qWarning() << "MapHandler: Failed to invoke addMarker";
        return;
    }

    int id = returnedValue.toInt();
    if (id >= 0) {
        qDebug() << "MapHandler: Added marker" << id << "at" << lat << lon;
        emit markerAdded(id, lat, lon);
    } else {
        qWarning() << "MapHandler: addMarker returned invalid ID:" << id;
    }
}

void MapHandler::updateMarker(int id, double lat, double lon)
{
    if (!m_mapItem) {
        qWarning() << "MapHandler: No map item set!";
        return;
    }

    bool success = QMetaObject::invokeMethod(m_mapItem, "updateMarker",
                                             Q_ARG(QVariant, id),
                                             Q_ARG(QVariant, lat),
                                             Q_ARG(QVariant, lon));

    if (success) {
        qDebug() << "MapHandler: Updated marker" << id << "to" << lat << lon;
        emit markerUpdated(id, lat, lon);
    } else {
        qWarning() << "MapHandler: Failed to update marker" << id;
    }
}

void MapHandler::removeMarker(int id)
{
    if (!m_mapItem) {
        qWarning() << "MapHandler: No map item set!";
        return;
    }

    bool success = QMetaObject::invokeMethod(m_mapItem, "removeMarker",
                                             Q_ARG(QVariant, id));

    if (success) {
        qDebug() << "MapHandler: Removed marker" << id;
        emit markerRemoved(id);
    } else {
        qWarning() << "MapHandler: Failed to remove marker" << id;
    }
}

void MapHandler::clearMarkers()
{
    if (!m_mapItem) {
        qWarning() << "MapHandler: No map item set!";
        return;
    }

    bool success = QMetaObject::invokeMethod(m_mapItem, "clearMarkers");

    if (success) {
        qDebug() << "MapHandler: Cleared all markers";
    } else {
        qWarning() << "MapHandler: Failed to clear markers";
    }
}