#pragma once

#include <QtCore/QObject>
#include <QtLocation/QGeoServiceProviderFactory>

class MapPlugin : public QObject, public QGeoServiceProviderFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.qt.geoservice.serviceproviderfactory/6.0" FILE "plugin.json")
    Q_INTERFACES(QGeoServiceProviderFactory)

public:
    QGeoMappingManagerEngine *createMappingManagerEngine(  
        const QMap<QString, QVariant> &parameters,        
        QGeoServiceProvider::Error *error,
        QString *errorString) const override;
};