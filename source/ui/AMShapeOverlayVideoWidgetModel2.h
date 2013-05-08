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
    int rectangleListLength(){return index_;}

    /// get the rectangle with given index
    QRectF rectangle(int index);

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
    void deleteRectangleVector();


protected:
    /// used for transforming points to where they actually appear
    QSizeF viewSize_;
    QSizeF scaledSize_;

    QRectF rectangle_;

    /// functions for getting the transformed locations of each rectangle
    QPointF rectangleTopLeft(int index);
    QPointF rectangleBottomRight(int index);

    /// highest current index
    int index_;
    /// index of the currently selected item
    int current_;
    /// vector from mouse tip to top left corner of current rectangle
    QPointF currentVector_;
    /// array of vectors from mouse tip to top left corner of every rectangle
    QPointF* rectangleVector_;

    /// The mapping of all the rectangles, indices go from 0 - index_
    /// for index_+1 rectangles
    QMap<int,AMShapeData2> rectangleList_;

    /// mouse location at start of a zoom process
    QPointF zoomPoint_;

    QPointF zoomCenter_;


    /// for ease, create a coordinate transform function
    /// takes a double transforms to where you are actually clicking
    /// as QPointF
    QPointF coordinateTransform(QPointF);

    QPointF center();

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

    void zoomAllRectangles(QPointF);

    void setZoomPoint(QPointF);

    void finishCurrentRectangle(QPointF);



};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
