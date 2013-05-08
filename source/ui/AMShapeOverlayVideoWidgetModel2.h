#ifndef AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
#define AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H

#include <QGraphicsView>
#include <QWidget>
#include <QObject>
#include <QSizeF>
#include <QMap>
#include "ui/AMShapeData2.h"

class AMShapeData2;
class QGraphicsRectItem;
class AMCrosshairOverlayVideoWidget2;

class AMShapeOverlayVideoWidgetModel2: public QObject
{
    Q_OBJECT
public:
    /// Constructor
    explicit AMShapeOverlayVideoWidgetModel2(QObject *parent = 0);


    /// viewSize and scaled_size used for computing coordinates, get values from AMCrosshairOverlayVideoWidget
    QSizeF viewSize(){return viewSize_;}
    QSizeF scaledSize(){return scaledSize_;}
    void setViewSize(QSizeF viewSize);
    void setScaledSize(QSizeF scaledSize);

    int rectangleListLength(){return index_;}

    QRectF rectangle(int index);

    QString currentName();
    void setCurrentName(QString name);
    QString currentInfo();
    void setCurrentInfo(QString info);

    int currentIndex();
    void setCurrentIndex(int current);
    bool isValid(int index);
    void deleteRectangleVector();


protected:
    QSizeF viewSize_;
    QSizeF scaledSize_;

    QRectF rectangle_;

    QPointF rectangleTopLeft(int index);
    QPointF rectangleBottomRight(int index);

    int index_;
    int current_;
    QPointF currentVector_;
    QPointF* rectangleVector_;

    QMap<int,AMShapeData2> rectangleList_;







    ///
    enum Coordinate { XCOORDINATE,YCOORDINATE};
    /// for ease, create a coordinate transform function
    /// takes a double transforms to where you are actually clicking
    /// as a doubles, must also pass wether it's x or y
    QPointF coordinateTransform(QPointF);

public slots:
    /// start position of rectangle
    void startRectangle(QPointF);

    /// end position of rectangle
    void finishRectangle(QPointF);

    /// delete a rectangle
    void deleteRectangle(QPointF);

    void selectCurrentRectangle(QPointF);

    void moveCurrentRectangle(QPointF);

    void moveAllRectangles(QPointF);

    void setRectangleVectors(QPointF);

    void finishCurrentRectangle(QPointF);



};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
