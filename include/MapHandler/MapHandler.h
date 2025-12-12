#pragma once
#include <QObject>
#include <QQuickItem>

class MapHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* mapItem READ mapItem WRITE setMapItem NOTIFY mapItemChanged)

public:
    static MapHandler* instance();

    QQuickItem* mapItem() const { return m_mapItem; }
    void setMapItem(QQuickItem* item);


    Q_INVOKABLE void addMarker(double lat, double lon, const QString& title = "");
    Q_INVOKABLE void updateMarker(int id, double lat, double lon);
    Q_INVOKABLE void removeMarker(int id);
    Q_INVOKABLE void clearMarkers();

signals:
    void mapItemChanged();
    void markerAdded(int id, double lat, double lon);
    void markerUpdated(int id, double lat, double lon);
    void markerRemoved(int id);

private:
    explicit MapHandler(QObject *parent = nullptr);
    QQuickItem* m_mapItem = nullptr;
};