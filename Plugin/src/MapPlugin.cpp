#include "MapPlugin.h"
#include "MapPluginEngine.h"



QGeoMappingManagerEngine *MapPlugin::createMappingManagerEngine(
    const QVariantMap &parameters,
    QGeoServiceProvider::Error *error,
    QString *errorString) const
{
    Q_UNUSED(errorString);

    *error = QGeoServiceProvider::NoError;
    return new MapPluginEngine(parameters);
}