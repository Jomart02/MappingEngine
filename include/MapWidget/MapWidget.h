#pragma once
#include <QQuickWidget>

class MapWidget : public QQuickWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
};