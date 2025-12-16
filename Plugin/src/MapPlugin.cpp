#include "MapPlugin.h"
#include "MapPluginEngine.h"



QGeoMappingManagerEngine *MapPlugin::createMappingManagerEngine(
    const QVariantMap &parameters,
    QGeoServiceProvider::Error *error,
    QString *errorString) const
{
    Q_UNUSED(errorString);


    qDebug() << "🔥 MapPlugin::createMappingManagerEngine CALLED";

    *error = QGeoServiceProvider::NoError;
    return new MapPluginEngine(parameters);
}