#pragma once
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

class MapReply: public QGeoTiledMapReply
{
    Q_OBJECT

public:
    MapReply(const QGeoTileSpec &spec,QObject *parent = nullptr);
    ~MapReply() override;
private:
     void setRedTile(const QGeoTileSpec &spec);
};

QT_END_NAMESPACE