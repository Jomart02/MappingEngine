#include "MapTileFetcher.h"
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QFile>
#include <QDir>
#include "MapPluginEngine.h"
#include "MapReply.h"


MapTileFetcher::MapTileFetcher(const QVariantMap &parameters,
    MapPluginEngine *engine,
    const QSize &tileSize)
    : QGeoTileFetcher(engine)
{

    
}
MapTileFetcher::~MapTileFetcher()
{
}

QGeoTiledMapReply * MapTileFetcher::getTileImage(const QGeoTileSpec &spec)
{

    qDebug() << "tile:" << spec.x() << spec.y() << spec.zoom();
    return new MapReply(spec, this);
}