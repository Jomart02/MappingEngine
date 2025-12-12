#pragma once
#include <QObject>
#include <QQuickItem>
#include <QMap>

class AbstractFeature;

class MapHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* mapItem READ mapItem WRITE setMapItem NOTIFY mapItemChanged)

public:
    static MapHandler* instance();

    QQuickItem* mapItem() const { return m_mapItem; }
    void setMapItem(QQuickItem* item);

    Q_INVOKABLE void addFeature(AbstractFeature* feature);
    Q_INVOKABLE void removeFeature(const QString& name);
    Q_INVOKABLE void removeFeature(AbstractFeature* feature);
    Q_INVOKABLE void clearAll();

    Q_INVOKABLE AbstractFeature* feature(const QString& name) const;
    Q_INVOKABLE QStringList featureNames() const { return m_features.keys(); }
    Q_INVOKABLE bool contains(const QString& name) const { return m_features.contains(name); }

    Q_INVOKABLE void setVisible(const QString& name, bool visible);
    Q_INVOKABLE void centerOn(const QString& name);

signals:
    void mapItemChanged();
    void featureAdded(const QString& name, int type, QVariantMap geometry, QVariantMap style);
    void featureUpdated(const QString& name, QVariantMap geometry, QVariantMap style);
    void featureRemoved(const QString& name);

private:
    explicit MapHandler(QObject* parent = nullptr);
    void setupConnections(AbstractFeature* f);

    QQuickItem* m_mapItem = nullptr;
    QMap<QString, AbstractFeature*> m_features;
};