#pragma once
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtCore/QPointer>
#include <QNetworkReply>
#include <QImage>

QT_BEGIN_NAMESPACE

class QNetworkReply;

class MapReply : public QGeoTiledMapReply
{
    Q_OBJECT
public:
    explicit MapReply(const QGeoTileSpec &spec, QObject *parent = nullptr);
    ~MapReply() override;

    void abort() override;

    void setOverlayEnabled(bool enabled) { m_overlayEnabled = enabled; }

private slots:
    void onOsmReplyFinished();
    void onSeaMapReplyFinished();

private:
    void finishWithImage(const QImage &image);
    QPointer<QNetworkReply> m_osmReply;
    QPointer<QNetworkReply> m_seaMapReply;
    QImage m_osmImage;  // Храним OSM изображение до загрузки второго слоя
    bool m_overlayEnabled = true;

    QString osmTileUrl(const QGeoTileSpec &spec) const;
    QString seaMapTileUrl(const QGeoTileSpec &spec) const;  // Новый URL для OpenSeaMap
};

QT_END_NAMESPACE