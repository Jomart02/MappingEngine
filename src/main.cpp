#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>
#include "MapWidget.h"
#include "MapHandler.h"
#include <QTimer>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    auto *layout = new QVBoxLayout(&window);
    auto *mapWidget = new MapWidget;

    layout->addWidget(mapWidget);
    window.resize(1000, 700);
    window.show();

    // Передаём карту в обработчик
    auto *mapItem = mapWidget->rootObject()->findChild<QQuickItem*>("mapItem");
    MapHandler::instance()->setMapItem(mapItem);
     const int TOTAL_MARKERS = 3000;
     
    // Пример использования
     QTimer::singleShot(1000, [](){
          int added = 0;
          for(int i = 0 ; i<= TOTAL_MARKERS ; i++){

               // Равномерное распределение по миру (чтобы было красиво)
               double progress = added / (double)TOTAL_MARKERS;
               double lat = -85 + 170 * progress;                                   // от -85 до +85
               double lon = -180 + 360 * ((added % 40) / 40.0);                     // красивые полосы
               ++added;
               QString title = QString("Маркер №%1").arg(added + 1, 4, 10, QChar('0'));
               MapHandler::instance()->addMarker(lat, lon, title);        
          }
     });

    return app.exec();
}