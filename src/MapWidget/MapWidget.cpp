#include "MapWidget.h"
#include <QQuickItem>
MapWidget::MapWidget(QWidget *parent)
    : QQuickWidget(parent)
{

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setSource(QUrl(QStringLiteral("qrc:/qml/Map.qml")));
    
}
