#pragma once

#include <QtLocation>
#include <QtLocation/private/qgeotilefetcher_p.h>
#include <QNetworkReply>

class QGeoTiledMapReply;
class MapPluginEngine;

class MapTileFetcher : public QGeoTileFetcher
{
    Q_OBJECT

public:
    explicit MapTileFetcher(const QVariantMap &parameters,MapPluginEngine *engine,const QSize &tileSize);
    ~MapTileFetcher();
    QGeoTiledMapReply* getTileImage(const QGeoTileSpec &spec);
private:
    QString m_offlineDir;
};