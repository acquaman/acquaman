#ifndef AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
#define AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H

#include <QGraphicsView>
#include <QWidget>
#include <QObject>
#include <QSizeF>
#include <QMap>
#include "ui/AMShapeData2.h"
#include <QVector>
#include <QVector3D>

class AMShapeData2;
class QGraphicsRectItem;
class QVector3D;
class QPolygonF;
class AMCameraConfigurationModel;
class AMBeamModel;

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

    /// get the shape with given index
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

    /// get or set rotation for the specified index, by default the active shape
    double rotation(int index = -1);
    void setRotation(double rotation, int index = -1);

    /// get or set tilt for the specified index, by default the active shape
    double tilt(int index = -1);
    void setTilt(double tilt, int index = -1);

    /// returns the current grouping rectangle
    QPolygonF groupRectangle();

    /// sets the motor cooridinates
    void setMotorCoordinate(double x, double y, double z, double r);

    /// gets each component of the motor coordinate
    double motorRotation();
    double motorX();
    double motorY();
    double motorZ();

    /// toggles distortion on or off
    void toggleDistortion();

    /// sets the current camera model
    void setCameraModel(AMCameraConfigurationModel*);

signals:
    void beamChanged(QObject*);
protected:
    /// used for transforming points to where they actually appear
    QSizeF viewSize_;
    QSizeF scaledSize_;

    /// defines the array positions of each point in a polygon rectangle
    static const int TOPLEFT;
    static const int TOPRIGHT;
    static const int BOTTOMRIGHT;
    static const int BOTTOMLEFT;
    static const int TOPCLOSE;

    /// the number of points in a rectangle
    static const int RECTANGLE_POINTS;

    /// constants for movement across the screen- used for approximation to actual distortion
    static const double X_XMOVEMENT;
    static const double X_YMOVEMENT;
    static const double Y_YMOVEMENT;
    static const double Y_XMOVEMENT;

    /// functions for getting the transformed locations of each rectangle
    QPointF shapeTopLeft(int index);
    QPointF shapeBottomRight(int index);

    /// highest current index
    int index_;
    /// index of the currently selected item
    int current_;
    /// vector for calculating shape movement
    QPointF currentVector_;

    /// The mapping of all the rectangles, indices go from 0 - index_
    /// for index_+1 rectangles
    QMap<int,AMShapeData2> shapeList_;

    /// mouse location at start of a zoom process
    QPointF zoomPoint_;

    /// Rectangle used for selecting a group of active rectangles
    QPolygonF groupRectangle_;

    /// indices of grouped rectangles
    int* groupList_;

    /// the motor coordinate
    QVector3D motorCoordinate_;

    /// motor rotation
    double motorRotation_;

    /// if true show distortion
    bool distortion_;

    /// the camera model
    AMCameraConfigurationModel* cameraModel_;

    /// the beam model
    AMBeamModel* beamModel_;

    /// intersections
    QVector<QPolygonF> intersections_;


    /// transforms point to where you are actually clicking
    QPointF coordinateTransform(QPointF);

    /// applies rotation to the shape at the given index
    QPolygonF applyRotation(QPolygonF shape, int index);

    /// rotate a single point
    QPointF getRotatedPoint(QPointF point, double z, double rotation, QPointF center);

    /// applies tilt to the shape at the given index
    /// takes QPolygonF to stack with rotation
    QPolygonF applyTilt(QPolygonF shape, int index);

    /// gets a tilted point
    QPointF getTiltedPoint(QPointF point, double z, double tilt, QPointF center);

    /// transforms the given point and depth to a 3D coordinate
    /// based on the current camera model
    QVector3D transform2Dto3D(QPointF point, double depth);

    /// transforms a 3D vector to a 2D point, using current camera configuration
    QPointF transform3Dto2D(QVector3D coordinate);

    /// builds a rectangle from the specified points
    QPolygonF constructRectangle(QPointF topLeft, QPointF bottomRight);

    /// returns the size of the rectangle specified by the two points
    QSizeF size(QPointF topLeft, QPointF bottomRight);

    /// returns whether the shape at the given index contains the given point
    bool contains(QPointF position,int index);

    /// rotates, tilts, and distorts the shape at index
    QPolygonF subShape(int index, QPolygonF shape);

    /// apply distortion to the current shape
    QPolygonF applyDistortion(QPolygonF shape);

    /// distort a point
    QPointF distortPoint(QPointF point);

    /// remove distortion from a shape
    QPolygonF removeDistortion(QPolygonF shape);

    /// remove distortion from a point
    QPointF undistortPoint(QPointF point);

    /// scales a length based on distance
    double transformDimension(double dimension, QVector3D coordinate);

    /// inverse scaling based on distance
    double inverseDimensionTransform(double dimension, QVector3D coordinate);

    /// scales a vector based on distance
    QPointF transformVector(QPointF vector, QVector3D coordinate);

    /// invers scaling of vector
    QPointF inverseVectorTransform(QPointF vector, QVector3D coordinate);

    /// finds the center of the polygon, assumes a rectangel
    QPointF findCenter(QPolygonF);

    /// shifts scene to correspond to motor movement
    void motorMovement(double x, double y, double z, double r);

    /// returns the depth of a given coordinate, based on camera model
    double depth(QVector3D);
    /// returns depth of a coordinate as a 3D vector
    QVector3D depthVector(QVector3D);

    QVector<QVector3D> findIntersectionShape(int index);

    QPolygonF intersectionScreenShape(QVector<QVector3D>);

public slots:
    /// start position of rectangle
    void startRectangle(QPointF);

    /// end position of rectangle
    void finishRectangle(QPointF);

    /// delete a rectangle
    void deleteRectangle(QPointF);

    /// selects a rectangle under the cursor
    void selectCurrentShape(QPointF);

    /// moves the current shape
    void moveCurrentShape(QPointF, int index = -1);

    /// moves all the shapes
    void moveAllShapes(QPointF);

    /// sets the offset to move shapes by
    void setShapeVectors(QPointF);

    /// zooms all shapes
    void zoomAllShapes(QPointF);

    /// initializes the point to zoom wrt
    void setZoomPoint(QPointF);

    /// apply rotation to the rectangle, by moving mouse sideways
    void rotateRectangle(QPointF);

    /// apply zoom, by moving mouse up/down
    void zoomShape(QPointF);

    /// sets the shapes coordinate
    void setCoordinates(QVector3D, int);

    /// set coordinates with doubles rather than a QVector3D
    void setCoordinates(double x, double y, double z, int index = -1);

    /// returns the coordinate of the given index
    QVector3D coordinate(int index);

    /// returns the coordinate of the current index
    QVector3D currentCoordinate();

    /// shifts all shapes so that position coincides with crosshairPosition
    void shiftToPoint(QPointF position, QPointF crosshairPosition);

    /// extends the group rectangle
    void finishGroupRectangle(QPointF position);

    /// creates the group rectangle
    void startGroupRectangle(QPointF position);

    /// places a grid (for distortion calibration)
    void placeGrid(QPointF position);

    /// updates the shape of the given index (it will not change or move until this is called)
    void updateShape(int index);

    /// updates all the shapes
    void updateAllShapes();

    /// finds the center cooridnate of the given index
    QVector3D centerCoordinate(int index);

    /// shifts all coordinates by a given amount
    void shiftCoordinates(QVector3D shift, int index);

    void oneSelect();
    void twoSelect();

    bool findIntersections();

    QVector<QPolygonF> intersections();

};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
