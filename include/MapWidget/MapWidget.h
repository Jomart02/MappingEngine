#pragma once
#include <QQuickWidget>

class MapWidget : public QQuickWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    QQuickItem* mapItem() const { return m_mapItem; }

private:
    QQuickItem* m_mapItem = nullptr;
};