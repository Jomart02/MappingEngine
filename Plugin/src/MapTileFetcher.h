#pragma once

#include <QtLocation>
#include <QtLocation/private/qgeotilefetcher_p.h>
#include <QNetworkReply>
#include "MapPluginEngine.h"

class QGeoTiledMapReply;
class MapPluginEngine;

class MapTileFetcher : public QGeoTileFetcher
{
    Q_OBJECT

public:
    explicit MapTileFetcher(const QVariantMap &parameters,MapPluginEngine *engine,const QSize &tileSize);
    ~MapTileFetcher();
    QGeoTiledMapReply* getTileImage(const QGeoTileSpec &spec);
    void setOverlay(bool over);
private:
    QString m_offlineDir;
    bool overlay = true;
    MapPluginEngine * m_engine;
};