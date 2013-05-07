#ifndef AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
#define AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H

#include <QGraphicsView>
#include <QWidget>
#include <QObject>
#include <QSizeF>

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

    double rectangleX();
    double rectangleY();
    double rectangleWidth();
    double rectangleHeight();


protected:
    double cursorX_, cursorY_;
    double rectangleTopX_, rectangleTopY_;
    double rectangleBottomX_,rectangleBottomY_;
    bool drawing_;
    QSizeF viewSize_;
    QSizeF scaledSize_;

    QGraphicsRectItem* rectangle_;



    ///
    enum Coordinate { XCOORDINATE,YCOORDINATE};
    /// for ease, create a coordinate transform function
    /// takes a double transforms to where you are actually clicking
    /// as a doubles, must also pass wether it's x or y
    double coordinateTransform(double, Coordinate);

public slots:
    /// start position of rectangle
    void startRectangle(QPointF);

    /// end position of rectangle
    void finishRectangle(QPointF);



};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
