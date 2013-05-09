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
class QVector3D;
class QPolygonF;

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

    /// get the current index of the rectangle List (number of rectangles - 1)
    int shapeListLength(){return index_;}

    /// get the rectangle with given index
    QPolygonF shape(int index);

    /// getters and setters for shape information
    QString currentName();
    void setCurrentName(QString name);
    QString currentInfo();
    void setCurrentInfo(QString info);

    /// index management
    int currentIndex();
    void setCurrentIndex(int current);
    bool isValid(int index);

    /// deletes the rectangle vectors array (used in movement)
    void deleteShapeVector();

    double rotation(int index = -1);
    void setRotation(double rotation, int index = -1);


protected:
    /// used for transforming points to where they actually appear
    QSizeF viewSize_;
    QSizeF scaledSize_;

    QRectF rectangle_;

    static const int TOPLEFT;
    static const int TOPRIGHT;
    static const int BOTTOMRIGHT;
    static const int BOTTOMLEFT;
    static const int TOPCLOSE;

    /// functions for getting the transformed locations of each rectangle
    QPointF shapeTopLeft(int index);
    QPointF shapeBottomRight(int index);

    /// highest current index
    int index_;
    /// index of the currently selected item
    int current_;
    /// vector from mouse tip to top left corner of current rectangle
    QPointF currentVector_;

    /// The mapping of all the rectangles, indices go from 0 - index_
    /// for index_+1 rectangles
    QMap<int,AMShapeData2> shapeList_;

    /// mouse location at start of a zoom process
    QPointF zoomPoint_;

    QPointF zoomCenter_;


    /// for ease, create a coordinate transform function
    /// takes a double transforms to where you are actually clicking
    /// as QPointF
    QPointF coordinateTransform(QPointF);

    double applyRotation(double width);

    QPolygonF constructRectangle(QPointF topLeft, QPointF bottomRight);

    QSizeF size(QPointF topLeft, QPointF bottomRight);



public slots:
    /// start position of rectangle
    void startRectangle(QPointF);

    /// end position of rectangle
    void finishRectangle(QPointF);

    /// delete a rectangle
    void deleteRectangle(QPointF);

    void selectCurrentShape(QPointF);

    void moveCurrentShape(QPointF);

    void moveAllShapes(QPointF);

    void setShapeVectors(QPointF);

    void zoomAllShapes(QPointF);

    void setZoomPoint(QPointF);


    void setCoordinates(QVector3D, int);

    void setCoordinates(double x, double y, double z, int index = -1);

    QVector3D coordinate(int index);

    QVector3D currentCoordinate();

    QPointF transform3Dto2D(QVector3D coordinate);

    double transformDimension(double dimension, QVector3D coordinate);

    void changeCoordinate();

};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
