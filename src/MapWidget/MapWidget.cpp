#include "MapWidget.h"

MapWidget::MapWidget(QWidget *parent)
    : QQuickWidget(parent)
{
    setSource(QUrl("qrc:/qml/Map.qml"));
    setResizeMode(QQuickWidget::SizeRootObjectToView);
}