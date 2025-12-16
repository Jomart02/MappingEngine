#include "MapPluginEngine.h"
#include "MapTileFetcher.h"
#include "QtLocation/private/qgeocameracapabilities_p.h"
#include "QtLocation/private/qgeomaptype_p.h"



MapPluginEngine::MapPluginEngine(const QVariantMap &parameters)
    : QGeoTiledMappingManagerEngine()
{
    qDebug() << "🔥 MapPluginEngine CONSTRUCTOR";
    QGeoCameraCapabilities cameraCaps;
    cameraCaps.setMinimumZoomLevel(0.0);
    cameraCaps.setMaximumZoomLevel(19.0);
    setCameraCapabilities(cameraCaps); //Задаем поддерживаемые картой уровни приближения
                                        //тут же можно заявить о поддержки всяких вкусняшек
                                        //по типу склонения карты относительно горизонта и т.п.

    setTileSize(QSize(256, 256));//Размер тайла. Выбран стандартный для OSM размер, 
                                 //который по утверждению самих OSM продиктован Google.
    setCacheHint(QAbstractGeoTileCache::CacheArea::MemoryCache);//Эта опция выставлена намерено,
                                                             //поскольку мы генерируем тайлы по данным с жесткого диска,
                                                             //хранить их в дисковом кэше смысла нет. 
    QList<QGeoMapType> types;

        types << QGeoMapType(
        QGeoMapType::StreetMap,         // style
        "Local tiles",                  // name
        "Offline local tiles",          // description
        false,                          // mobile
        false,                          // night
        1,                              // mapId
        QByteArrayLiteral("MapPlugin"), // 🔥 ОБЯЗАТЕЛЬНО совпадает с JSON Keys
        cameraCaps,
        QVariantMap()                   // metadata
    );
    setSupportedMapTypes(types);

    m_tileFetcher = new MapTileFetcher(parameters, this, QSize(256, 256));
    setTileFetcher(m_tileFetcher);


}

QGeoMap *MapPluginEngine::createMap()
{
    // Используем QGeoTiledMap — стандартная карта для плиток
    return new QGeoTiledMap(this, nullptr);
}