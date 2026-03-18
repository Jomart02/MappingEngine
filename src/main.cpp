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

    qDebug() << QGeoServiceProvider::availableServiceProviders();
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
    qDebug() << engine.rootObjects();
    if (engine.rootObjects().isEmpty())
        return -1;

    
     
    QWidget window;
    window.setWindowTitle("MappingEngine — Профессиональный стиль");
    auto *layout = new QVBoxLayout(&window);
    auto *mapWidget = new MapWidget;
    layout->addWidget(mapWidget);
    window.resize(1400, 900);
    window.show();
    
    
    
    auto* handler = MapHandler::instance();
    
    // 1. ADD INDIVIDUAL FEATURE (Line)
    QTimer::singleShot(1000, [handler](){
        qDebug() << "Test 1: Adding Route Line";
        auto* line = new LineString(); 
        line->coordinates << Coordinate(37.6173, 55.7558) << Coordinate(47.7, 55.8) << Coordinate(12.6173, 15.7558);
        
        auto* feature = new LineFeature("route_1", line);
        feature->style()->setColor( Qt::red) ;
        feature->style()->setWidth(5);
        handler->addFeature(feature);
    });

   // 2. MODIFY FEATURE (Animation)
    QTimer::singleShot(5000, [handler](){
        qDebug() << "Test 2: Modifying Route Geometry & Style";
        if(auto* f = handler->feature("route_1")) {
            // Change Style
            f->styleAs<LineStyle>()->setColor( Qt::green);
            f->styleAs<LineStyle>()->setWidth( 10);
            f->styleAs<LineStyle>()->setDashPattern({10.0, 50.0});
            f->notifyStyleChanged();

            // // Change Geometry (Move end point)
            auto* geom = static_cast<LineString*>(f->geometry());
            geom->coordinates[1] = Coordinate(37.8, 55.6); 
            f->notifyGeometryChanged();
        }
    });

    // 3. CREATE A GROUP AND ADD TO MAP
    // Keep pointer to group safe (usually stored in a class, here capture in lambda)
    static FeatureGroup* cityGroup = new FeatureGroup("city_objects");

    QTimer::singleShot(3000, [handler](){
        qDebug() << "Test 3: Adding Group with existing items";
        
        // Add Circle to Group
        auto* circle = new Circle({37.65, 55.72}, 2000);
        auto* fCircle = new CircleFeature("zone_A", circle);
        fCircle->styleAs<PolygonStyle>()->setFillColor(QColor(255, 0, 0, 100));
        cityGroup->addFeature(fCircle);

        // Add Group to Map (Circle should appear)
        handler->addGroup(cityGroup);
    });

    // 4. ADD ITEM TO GROUP ALREADY ON MAP
    // QTimer::singleShot(4000, [handler](){
    //     qDebug() << "Test 4: Adding item to existing group on map";
        
    //     auto* poly = new Polygon();
    //     poly->exteriorRing << Coordinate(37.5, 55.7) << Coordinate(37.55, 55.7) << Coordinate(37.52, 55.75);
    //     auto* fPoly = new PolygonFeature("zone_B", poly);
        
    //     // This should immediately trigger MapHandler to add it to QML
    //     cityGroup->addFeature(fPoly); 
    // });

    // // 5. HIDE GROUP
    QTimer::singleShot(5000, [handler](){
        qDebug() << "Test 5: Hiding Group";
        cityGroup->setVisible(false);
    });

    // // 6. REMOVE GROUP (Removes items from map)
    // QTimer::singleShot(6000, [handler](){
    //     qDebug() << "Test 6: Removing Group";
    //     handler->removeGroup("city_objects");
    // });

    return app.exec();
}


