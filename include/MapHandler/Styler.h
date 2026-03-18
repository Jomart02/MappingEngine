#pragma once
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QVariantMap>


class Styler
{
public:
    virtual ~Styler() = default;

    bool visible = true;
    int zIndex = 10;
    qreal opacity = 1.0;


    virtual QVariantMap toVariantMap() const = 0;
};


class PointStyle : public Styler
{
public:
    QString iconPath = "qrc:/icons/marker.png";
    qreal iconScale = 1.0;

     QVariantMap toVariantMap() const override {
          return {
               {"icon", iconPath},
               {"iconScale", iconScale},
               {"visible", visible},
               {"opacity", opacity},
               {"z", zIndex}
          };
     }
};


class LineStyle : public Styler
{
public:
    QPen pen = QPen(QColor("#3388ff"), 5, Qt::SolidLine);

    void setColor(const QColor& c) { pen.setColor(c); }
    void setWidth(qreal w) { pen.setWidthF(w); }
    void setDashPattern(const QVector<qreal>& dashes) {   
        pen.setStyle(Qt::CustomDashLine);
        pen.setDashPattern(dashes);  
    } 

    QVariantMap toVariantMap() const override {
        QVariantMap result;
        result["color"] = pen.color().name();
        result["weight"] = pen.widthF();
        
        QVector<qreal> dashes = pen.dashPattern();
        QStringList dashStrings;
        for (qreal d : dashes) {
            dashStrings << QString::number(d);
        }
        result["dashArray"] = dashStrings.join(",");
        
        result["visible"] = visible;
        result["opacity"] = opacity;
        result["z"] = zIndex;
        return result;
    }
};


class PolygonStyle : public Styler
{
public:
    QPen borderPen = QPen(QColor("#3388ff"), 3, Qt::SolidLine);
    QBrush fillBrush = QBrush(QColor("#3388ff44"));  

    void setBorderColor(const QColor& c) { borderPen.setColor(c); }
    void setBorderWidth(qreal w) { borderPen.setWidthF(w); }
    void setFillColor(const QColor& c) { fillBrush.setColor(c); }
    void setFillPattern(Qt::BrushStyle pattern) { fillBrush.setStyle(pattern); }  

    QVariantMap toVariantMap() const override {
        return {
            {"color", borderPen.color().name()},
            {"weight", borderPen.widthF()},
            {"fillColor", fillBrush.color().name()},
            {"visible", visible},
            {"opacity", opacity},
            {"z", zIndex}
        };
    }
};


class CircleStyle : public PolygonStyle 
{
public:
    QVariantMap toVariantMap() const override {
        return PolygonStyle::toVariantMap();  
    }
};