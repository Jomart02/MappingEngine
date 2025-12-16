
#include "MapReply.h"
#include <QImage>
#include <QBuffer>

MapReply::MapReply(const QGeoTileSpec &spec, QObject *parent):
    QGeoTiledMapReply(spec, parent)
{
     setRedTile(spec);
}
MapReply::~MapReply() = default;

void MapReply::setRedTile(const QGeoTileSpec &spec)
{
    const int size = 256;
    QImage image(size, size, QImage::Format_ARGB32);


    int r = (spec.x() * 37) % 256;
    int g = (spec.y() * 53) % 256;
    int b = (spec.zoom() * 40) % 256;

    QColor color(r, g, b);
    image.fill(color);

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    setMapImageData(data);
    setMapImageFormat("png");
    setFinished(true);
}