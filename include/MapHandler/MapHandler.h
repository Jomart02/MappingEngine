#pragma once
#include <QObject>
#include <QQuickItem>
#include <QMap>

class AbstractFeature;
class FeatureGroup;
class Geometry;

class MapHandler : public QObject
{
    Q_OBJECT
public:
    static MapHandler* instance();
    
    Q_INVOKABLE void addFeature(AbstractFeature* feature);
    Q_INVOKABLE void removeFeature(const QString& name);
    Q_INVOKABLE void removeFeature(AbstractFeature* feature);
    Q_INVOKABLE void clearAll();

    Q_INVOKABLE AbstractFeature* feature(const QString& name) const;
    Q_INVOKABLE QStringList featureNames() const { return m_features.keys(); }
    Q_INVOKABLE bool contains(const QString& name) const { return m_features.contains(name); }

    Q_INVOKABLE void addGroup(FeatureGroup* group);
    Q_INVOKABLE void removeGroup(const QString& groupName);
    Q_INVOKABLE FeatureGroup* getGroup(const QString& name) const;

    Q_INVOKABLE void setVisible(const QString& name, bool visible);
    Q_INVOKABLE void centerOn(const QString& name);

signals:
    void featureAdded(const QString& name, int type, QVariantMap geometry, QVariantMap style);
    void featureUpdated(const QString& name, int type, QVariantMap geometry, QVariantMap style);
    void featureRemoved(const QString& name);
    void requestMapRefresh();
private slots:
    void onFeatureChanged();
private:
    explicit MapHandler(QObject* parent = nullptr);
    void setupConnections(AbstractFeature* f);
    QVariantMap geometryToVariant(Geometry* g) const;

    QMap<QString, AbstractFeature*> m_features;
    QMap<QString, FeatureGroup*> m_groups;
};