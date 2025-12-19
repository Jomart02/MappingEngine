
#include "MapReply.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QImage>
#include <QPainter>
#include <QBuffer>


MapReply::MapReply(const QGeoTileSpec &spec, QObject *parent)
    : QGeoTiledMapReply(spec, parent)
    , m_osmImage()
{
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

    // Загружаем OSM (база)
    {
        QUrl url(osmTileUrl(spec));
        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", "MyQtMapApp/1.0 (contact@example.com)");

        m_osmReply = nam->get(request);
        connect(m_osmReply, &QNetworkReply::finished, this, &MapReply::onOsmReplyFinished);
    }

    // Если оверлей включён — загружаем OpenSeaMap
    if (m_overlayEnabled) {
        QUrl url(seaMapTileUrl(spec));
        QNetworkRequest request(url);
        request.setRawHeader("User-Agent", "MyQtMapApp/1.0 (contact@example.com)");

        m_seaMapReply = nam->get(request);
        connect(m_seaMapReply, &QNetworkReply::finished, this, &MapReply::onSeaMapReplyFinished);
    }
}

MapReply::~MapReply()
{
    if (m_osmReply && m_osmReply->isRunning()) m_osmReply->abort();
    if (m_seaMapReply && m_seaMapReply->isRunning()) m_seaMapReply->abort();
}

void MapReply::abort()
{
    if (m_osmReply && m_osmReply->isRunning()) m_osmReply->abort();
    if (m_seaMapReply && m_seaMapReply->isRunning()) m_seaMapReply->abort();
}

void MapReply::onOsmReplyFinished()
{
    if (!m_osmReply || m_osmReply->error() != QNetworkReply::NoError) {
        setError(CommunicationError, m_osmReply ? m_osmReply->errorString() : "No OSM reply");
        setFinished(true);
        return;
    }

    QByteArray data = m_osmReply->readAll();
    if (!m_osmImage.loadFromData(data)) {
        setError(ParseError, "Failed to load OSM tile");
        setFinished(true);
        return;
    }

    // Конвертируем в ARGB32 для рисования
    if (m_osmImage.format() != QImage::Format_ARGB32) {
        m_osmImage = m_osmImage.convertToFormat(QImage::Format_ARGB32);
    }

    // Если оверлей не нужен — сразу завершаем
    if (!m_overlayEnabled || !m_seaMapReply) {
        finishWithImage(m_osmImage);
        return;
    }

    // Иначе ждём второй слой (onSeaMapReplyFinished проверит, загружен ли уже)
    if (m_seaMapReply && m_seaMapReply->isFinished()) {
        onSeaMapReplyFinished();  // Если второй уже готов
    }
}

void MapReply::onSeaMapReplyFinished()
{
   QImage finalImage = m_osmImage;  // Копируем базу (уже в ARGB32 и валидная)

    if (m_seaMapReply) {
        if (m_seaMapReply->error() == QNetworkReply::NoError) {
            QByteArray seaData = m_seaMapReply->readAll();
            QImage seaImage;
            if (seaImage.loadFromData(seaData) && !seaImage.isNull()) {
                // Конвертируем только если нужно
                if (seaImage.format() != QImage::Format_ARGB32) {
                    seaImage = seaImage.convertToFormat(QImage::Format_ARGB32);
                }

                QPainter painter(&finalImage);
                painter.drawImage(0, 0, seaImage);
                painter.end();  // Хорошая практика
            }
            // Если loadFromData() провалилось или isNull() — просто игнорируем (пустой тайл)
        } else if (m_seaMapReply->error() == QNetworkReply::ContentNotFoundError) {
            // 404 — нормально для OpenSeaMap, ничего не рисуем
        } else {
            qDebug() << "OpenSeaMap error:" << m_seaMapReply->errorString();
        }
    }

    finishWithImage(finalImage);
}



// Вспомогательная функция для завершения
void MapReply::finishWithImage(const QImage &image)
{
    QByteArray resultData;
    QBuffer buffer(&resultData);
    buffer.open(QIODevice::WriteOnly);
    if (image.save(&buffer, "PNG")) {
        setMapImageData(resultData);
        setMapImageFormat("png");
    } else {
        setError(UnknownError, "Failed to save combined tile");
    }
    setFinished(true);
}

QString MapReply::osmTileUrl(const QGeoTileSpec &spec) const
{
    int x = spec.x();
    int y = spec.y();
    int z = spec.zoom();
    char subdomain = 'a' + (x + y + z) % 3;
    return QStringLiteral("https://%1.tile.openstreetmap.org/%2/%3/%4.png")
           .arg(QChar(subdomain)).arg(z).arg(x).arg(y);
}

QString MapReply::seaMapTileUrl(const QGeoTileSpec &spec) const
{
    int x = spec.x();
    int y = spec.y();
    int z = spec.zoom();
    // Официальный и рабочий URL OpenSeaMap (прозрачные тайлы с морскими знаками)
    return QStringLiteral("https://tiles.openseamap.org/seamark/%1/%2/%3.png")
           .arg(z).arg(x).arg(y);
}