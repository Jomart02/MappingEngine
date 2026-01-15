#pragma once

#include "QtLocation/private/qgeotiledmappingmanagerengine_p.h"
#include <QtPositioning/QGeoRectangle>

#include "QtLocation/private/qgeotiledmap_p.h"
#include <QVariantMap>

class MapTileFetcher;
class MapPluginEngine : public QGeoTiledMappingManagerEngine
{
    Q_OBJECT
public:
    explicit MapPluginEngine(const QVariantMap &parameters);
    QGeoMap *createMap() override;
    static MapPluginEngine* instance();
    MapTileFetcher* getFetcher();
private:
    static MapPluginEngine* s_instance;
    MapTileFetcher *m_tileFetcher = nullptr;
};