// main.cpp — ФИНАЛЬНАЯ ВЕРСИЯ, БЕЗ ОШИБОК
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MapHandler.h"
#include "Feature.h"
#include "Geometry.h"
#include "MapWidget.h"
#include <QVBoxLayout>
#include <QTimer>

#include <QGeoServiceProvider>
 // qDebug() << QGeoServiceProvider::availableServiceProviders();


int main(int argc, char *argv[])
{
QApplication app(argc, argv);


    qmlRegisterUncreatableType<GeometryType>("com.mycompany.map", 1, 0, "GeometryType", "Enum only");

    qmlRegisterSingletonType<MapHandler>("com.mycompany.map", 1, 0, "MapHandler",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            MapHandler* h = MapHandler::instance();
            QQmlEngine::setObjectOwnership(h, QQmlEngine::CppOwnership);
            return h;
        });

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/Map.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    auto* handler = MapHandler::instance();
     
     QWidget window;
    window.setWindowTitle("MappingEngine — Профессиональный стиль");
    auto *layout = new QVBoxLayout(&window);
    auto *mapWidget = new MapWidget;
    layout->addWidget(mapWidget);
    window.resize(1400, 900);
    window.show();

    auto* lineGeom = new LineString();
    lineGeom->coordinates << Coordinate(17.58, 25.73)
                          << Coordinate(37.72, 55.76)
                          << Coordinate(67.65, 45.80);

    auto* route = new LineFeature("main_route", lineGeom);
    auto* lineStyle = route->style();  
    lineStyle->pen.setColor(QColor(Qt::red));
    lineStyle->pen.setWidthF(1);
    lineStyle->pen.setDashPattern({20, 10, 5, 10}); 
    lineStyle->opacity = 0.9;
    lineStyle->zIndex = 100;
    handler->addFeature(route);

    auto* polyGeom = new Polygon();
    polyGeom->exteriorRing << Coordinate(37.60, 55.70)
                           << Coordinate(2.75, 5.70)
                           << Coordinate(10.70, 10.78)
                           << Coordinate(37.60, 55.70);

    auto* zone = new PolygonFeature("protected_zone", polyGeom);
    auto* polyStyle = zone->style();
    polyStyle->borderPen.setColor(Qt::darkBlue);
    polyStyle->borderPen.setWidthF(6);
    polyStyle->fillBrush.setColor(QColor("#2a9d8f88"));
    polyStyle->fillBrush.setStyle(Qt::Dense4Pattern);  // сетка из точек
    polyStyle->opacity = 0.7;
    handler->addFeature(zone);

    // === ТЕСТ 4: Круг с градиентной заливкой (через QColor) ===
    auto* circleGeom = new Circle({37.62, 55.75}, 5000);  // 5 км
    auto* coverage = new CircleFeature("signal_coverage", circleGeom);
    auto* circleStyle = coverage->style();
    circleStyle->borderPen.setColor(QColor("#ff0066"));
    circleStyle->borderPen.setWidthF(8);
    circleStyle->borderPen.setDashPattern({15, 8});  // пунктир
    circleStyle->fillBrush.setColor(QColor("#ff006644"));
    circleStyle->opacity = 0.6;
    handler->addFeature(coverage);

//     
    QTimer::singleShot(1000, [=]() mutable {
        for (int i = 0; i < 5; ++i) {
            QTimer::singleShot(i * 300, [=]() {
                auto* pulse = new CircleFeature(QString("pulse_%1").arg(i),
                                               new Circle({37.6173 + i*0.01, 55.7558 + i*0.01}, 1000 + i*500));
                auto* s = pulse->style();
                s->borderPen.setColor(QColor(255, 255, 255, 200));
                s->borderPen.setWidthF(4 + i);
                s->fillBrush.setColor(QColor(100, 200, 255, 30 + i*20));
                s->opacity = 0.8;
                handler->addFeature(pulse);
                    QTimer::singleShot(3000, [=]() {
                         s->borderPen.setColor(QColor(Qt::red));
                         s->borderPen.setWidthF(4*5 + i);
                         s->fillBrush.setColor(QColor(100, 200, 255, 30 + i*20));
                         s->opacity = 0.8;
                          pulse->setStyle(s);
                    });
                
                QTimer::singleShot(7000, [=]() {
                    handler->removeFeature(pulse);
                });
            });
        }
    });




    return app.exec();
}