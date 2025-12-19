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

    qDebug() << "Request tile:" << spec.x() << spec.y() << spec.zoom();

    MapReply *reply = new MapReply(spec, this);

    // ВКЛЮЧАЕМ ВАШ ОВЕРЛЕЙ ЗДЕСЬ
    // Вариант 1: всегда включён
    // reply->setOverlayEnabled(true);

    // Вариант 2: по параметру плагина (рекомендуется)
    bool overlayEnabled = parent()->property("overlayEnabled").toBool(); // если используете dynamic property
    // или через parameters, переданные в engine

    reply->setOverlayEnabled(true);  // <-- Включите, когда нужно показать вашу карту

    return reply;
}