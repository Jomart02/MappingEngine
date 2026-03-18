#include "MapTileFetcher.h"
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QFile>
#include <QDir>
#include "MapPluginEngine.h"
#include "MapReply.h"


MapTileFetcher::MapTileFetcher(const QVariantMap &parameters,
    MapPluginEngine *engine,
    const QSize &tileSize)
    : QGeoTileFetcher(engine),m_engine(engine)
{

    
}
MapTileFetcher::~MapTileFetcher()
{
}

void MapTileFetcher::setOverlay(bool over){
    overlay = over;
    m_engine->tileCache()->clearAll();
}

QGeoTiledMapReply * MapTileFetcher::getTileImage(const QGeoTileSpec &spec)
{

    // qDebug() << "Request tile:" << spec.x() << spec.y() << spec.zoom();
    MapReply *reply = new MapReply(spec, overlay,this);
    // qDebug() << overlay;
    // reply->setOverlayEnabled(overlay);  // <-- Включите, когда нужно показать вашу карту

    return reply;
}