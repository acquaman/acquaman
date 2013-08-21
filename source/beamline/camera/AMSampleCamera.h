#ifndef AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
#define AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H

#include <QGraphicsView>
#include <QWidget>
#include <QObject>
#include <QSizeF>
#include <QMap>
#include "ui/AMShapeData.h"
#include <QVector>
#include <QVector3D>

#include "ui/AMBeamConfiguration.h"
#include <QPair>
#include <complex>
#include <QGenericMatrix>

#include <Eigen/SVD>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/LU>

#define SAMPLEPOINTS 6

class AMShapeData;
class QGraphicsRectItem;
class QVector3D;
class QPolygonF;
class AMCameraConfiguration;
//class SGMMAXvMotor;
class AMControl;
class AMCamera;

using namespace Eigen;

class AMSampleCamera: public QAbstractListModel
{
    Q_OBJECT
public:

    static AMSampleCamera* set();

    enum AxisDirection {XAXIS,YAXIS,ZAXIS};

    /// Accessors

    /// member accessors -----------------------------------------------------------------------------------

    /// get the current index of the rectangle List (number of rectangles - 1)
    int shapeListLength() const;

    /// get the current index
    int currentIndex() const;

    /// get the camera configuration
    AMCameraConfiguration *cameraConfiguration() const;

    /// get the beam configuration
    AMBeamConfiguration *beamConfiguration() const;

    /// returns the current grouping rectangle
    QPolygonF groupRectangle() const;

    /// gets each component of the motor coordinate
    double motorX() const;
    double motorY() const;
    double motorZ() const;

    double motorRotation() const;

    /// get a list of all the intersections with the beam
    QVector<QPolygonF> intersections() const;

    /// returns the crosshair location
    QPointF crosshair() const;
    double crosshairX() const;
    double crosshairY() const;

    /// is the crosshair locked?
    bool crosshairLocked() const;

    /// viewSize and scaled_size used for computing coordinates, get values from AMShapeDataSetGraphicsView
    QSizeF viewSize() const;

    QSizeF scaledSize() const;

    /// true if using cameraMatrix for transforms
    bool useCameraMatrix() const;

    QPolygonF currentPolygon() const;

    /// ------------------------------------------------------------------------------------------------


    /// other accessors
    QString currentName() const;
    QString name(int index) const;
    QString currentInfo() const;
    QString otherData(int index) const;
    double idNumber(int index) const;
    bool visible() const;
    bool visible(int index) const;

    double rotation(int index) const;
    double rotation() const;
    double tilt(int index) const;
    double tilt() const;

    /// the AMShapeData with given index
    AMShapeData *currentShape(int index) const;
    AMShapeData *currentShape() const;

    /// the index of the given shape
    int indexOfShape(AMShapeData* shape) const;


    /// get the shape with given index
    QPolygonF shape(int index) const;


    /// returns the coordinate of the given index
    QVector3D coordinate(int index) const;

    /// returns the coordinate of the current index
    QVector3D currentCoordinate() const;

    /// -----------------------------------------------------------------------------------




public slots:

    /// Mutators

    /// sets the current index to current
    void setCurrentIndex(int current);

    /// sets the current camera model
    void setCameraModel(AMCameraConfiguration*);

    /// sets the current beam model
    void setBeamModel(AMBeamConfiguration*);

    /// sets the motor cooridinates
    void setMotorCoordinate(double x, double y, double z, double r);

    /// toggles distortion on or off
    void toggleDistortion();

    /// crosshair
    void setCrosshair(QPointF crosshair);
    void setCrosshairX(double x);
    void setCrosshairY(double y);

    void setCrosshairLocked(bool locked);

    void setViewSize(QSizeF viewSize);

    void setScaledSize(QSizeF scaledSize);


    /// setters for shape elements
    void setCurrentName(QString name);
    void setCurrentInfo(QString info);
    void setRotation(double rotation, int index = -1);
    void setTilt(double tilt, int index = -1);
    void setName(QString name, int index);
    void setOtherData(QString data, int index);
    void setIdNumber(double number, int index);
    void setVisible(bool visible);
    void setVisible(bool visible, int index);

    /// sets whether to use camera matrix for transforms
    void setUseCameraMatrix(bool use);

    void setOverrideMouseSelection(bool overrideMouseSelection);

    void setCurrentShapeIndex(int index);



public:
    /// public functions

    /// checks for valid index
    bool isValid(int index) const;

    /// checks if shape is facing away from camera
    bool isBackwards(int index) const;
    bool isBackwards() const;

    /// finds camera parameters
    void findCamera(QPointF points [SAMPLEPOINTS], QVector3D coordinates[SAMPLEPOINTS]);

    /// look for intersections with the current beam
    bool findIntersections();

    void deleteShape(int index);

    int samplePlateIndex() const;


    /// list model functions
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;


    bool motorMovementenabled();

     const QList<AMShapeData*> shapeList();

     bool overrideMouseSelection();



public slots:

    /// Mouse interactions--------------------------------------------------------------------------------

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

    /// shifts all shapes so that position coincides with crosshairPosition
    void shiftToPoint(QPointF position, QPointF crosshairPosition);

    /// extends the group rectangle
    void finishGroupRectangle(QPointF position);

    /// creates the group rectangle
    void startGroupRectangle(QPointF position);

    /// draws a non-rectangular shape
    void drawShape(QPointF position);

    /// completes the non-rectangular shape currently being drawn
    void finishShape();


    /// --------------------------------------------------------------------------------

    /// starts the drawing of polygon
    void startMultiDraw();


    /// updates the shape of the given index (it will not change or move until this is called)
    void updateShape(int index);

    /// updates all the shapes
    void updateAllShapes();

    /// sets the shapes coordinate
    void setCoordinates(QVector3D, int);

    /// set coordinates with doubles rather than a QVector3D
    void setCoordinates(double x, double y, double z, int index = -1);

    /// places a grid (for distortion calibration)
    void placeGrid(QPointF position);

    /// selects one of the beam shapes
    void oneSelect();
    /// selects the second beam shape
    void twoSelect();

    /// enables motor moving
    void enableMotorMovement(bool isEnabled);

    /// enables motor tracking
    void enableMotorTracking(bool isEnabled);

    /// deletes the shapes used for calibration
    void deleteCalibrationPoints();

    /// sets the shape to draw on
    void setDrawOnShape();

    /// sets the shape as SamplePlate
    void setDrawOnSamplePlate();

    /// enables drawing on the set shap
    void setDrawOnShapeEnabled(bool enable);

    void setBeamMarker(QPointF position, int index);
    void updateBeamMarker(QPointF position, int index);

    void beamCalibrate();

    void setSamplePlate();

    void setCameraConfigurationShape();

    void moveSamplePlateTo(QVector3D coordinate);

    /// move the sample plate by a small amount, proportional to movement
    void moveSamplePlate(int movement);

    void addBeamMarker(int index);

    void updateView();

    void moveMotorTo(QVector3D coordinate);

    void stopMotors();





signals:
    /// used to change the beam
    void beamChanged(QObject*);

    /// update coordinates
    void motorMoved();

    void moveSucceeded();

    void shapesChanged();

    void currentIndexChanged(int);

    void shapeNameChanged(QString newName);

protected slots:
    /// tracks the motor location
    void motorTracking(double);

    /// checks to see if the motor is finished moving
    void motorsFinishedMoving();

protected:

    /// Constructor
    explicit AMSampleCamera(QObject *parent = 0);


    /// Manipulations


    /// shifts all coordinates by a given amount
    void shiftCoordinates(QVector3D shift, int index);

    /// apply the rotation in the given direction by the given angle to the given shape
    AMShapeData* applySpecifiedRotation(const AMShapeData* shape, QVector3D direction, double angle) const;

    /// convenience function for above apply specified rotation.
    /// applies axis rotation to the shape, in direction XAXIS, YAXIS, or  ZAXIS, by the amount given in shape.
    AMShapeData* applySpecifiedRotation(const AMShapeData* shape, AxisDirection direction) const;

    /// general rotation of a point about a point along the direction
    QVector3D rotateCoordinate(QVector3D coordinate, QVector3D center, QVector3D direction, double rotation) const;


    /// rotates, tilts, and distorts the shape at index
    QPolygonF subShape(const AMShapeData* shape) const;

    /// apply distortion to the current shape
    QPolygonF applyDistortion(QPolygonF shape) const;


    /// remove distortion from a shape
    QPolygonF removeDistortion(QPolygonF shape) const;

    /// remove distortion from a point
    QPointF undistortPoint(QPointF point) const;

    /// shifts scene to correspond to motor movement
    void motorMovement(double x, double y, double z, double r);

    /// rotates the given shape
    QVector<QVector3D> rotateShape(const AMShapeData* shape) const;


    /// Transformations

    /// transforms point to where you are actually clicking
    QPointF coordinateTransform(QPointF) const;


    /// Helper functions

    /// converts a shape from normalized coordinates to an actual screen position
    QPolygonF screenShape(QPolygonF shape) const;

    /// builds a rectangle from the specified points
    QPolygonF constructRectangle(QPointF topLeft, QPointF bottomRight) const;

    /// returns the size of the rectangle specified by the two points
    QSizeF size(QPointF topLeft, QPointF bottomRight) const;

    /// returns whether the shape at the given index contains the given point
    bool contains(QPointF position,int index) const;

    /// returns the depth of a given coordinate, based on camera model
    double depth(QVector3D) const;

    /// returns depth of a coordinate as a 3D vector
    QVector3D depthVector(QVector3D) const;

    /// finds the intersection of the shape with the beam
    QVector<QVector3D> findIntersectionShape(int index) const;

    /// converts the intersection shape to a shape on the screen
    QPolygonF intersectionScreenShape(QVector<QVector3D>) const;

    /// finds the center coordinate of the given index
    QVector3D centerCoordinate(int index) const;

    /// finds the dot product of two vectors
    double dot(QVector3D,QVector3D) const;

    /// returns the unit vector in the direction of the last line in the shape.
    /// for a rectangle this is the "upward" direction
    QVector3D getHeightNormal(const AMShapeData* shape) const;
    /// returns the unit vector in the direction of the first line in the shape
    /// for a rectangle this is the "right" direction
    QVector3D getWidthNormal(const AMShapeData* shape) const;
    /// returns the vector normal to heightVector and widthVector
    QVector3D getNormal(QVector3D heightVector, QVector3D widthVector) const;
    /// returns the vector normal to the given shape
    QVector3D getNormal(const AMShapeData* shape) const;
    /// returns the 3D point that lies under position and on the plane defined by the drawOnShape_ shape
    QVector3D getPointOnShape(QPointF position,QVector3D normal) const;

    /// returns the unit vector in the direction of the top of the screen to the bottom
    QVector3D downVector() const;
    /// returns the unit vector in the direction from the left of the screen to the right
    QVector3D rightVector() const;

    /// inserts an item into the shape list - use this rather than inserting
    /// manually into the list - keeps the model updated
    void insertItem(AMShapeData* item);

    /// removes an item from the shape list - use this rather than
    /// removing items manually - keeps the model updated
    void removeItem(int index);

    /// removes an item from the shape list and returns it
    /// use this rather than taking manually - keeps the model updated
    AMShapeData* takeItem(int index);

    /// used to find the point to move the plate to, to position beneath the beam
    QVector3D beamIntersectionPoint(QVector3D samplePoint);


    bool moveMotors(double x, double y, double z);








protected:

    /// Members

    static AMSampleCamera* set_;

    /// The mapping of all the rectangles, indices go from 0 - index_
    /// for index_+1 rectangles
    QList<AMShapeData*> shapeList_;

    /// highest current index
    int index_;

    /// index of the currently selected item
    int currentIndex_;

    /// The camera model
    AMCamera* camera_;

    /// the beam model
    AMBeamConfiguration* beamModel_;

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

    /// intersections
    QVector<QPolygonF> intersections_;

    /// the position of the crosshair
    QPointF crosshair_;

    bool crosshairLocked_;


    /// used for transforming points to where they actually appear
    QSizeF viewSize_;
    QSizeF scaledSize_;

    /// vector for calculating shape movement
    QPointF currentVector_;

    /// mouse location at start of a zoom process
    QPointF zoomPoint_;

    /// motor manipulators

    /// X motor
    //SGMMAXvMotor *ssaManipulatorX_;
    AMControl *ssaManipulatorX_;

    /// Y motor
    //SGMMAXvMotor *ssaManipulatorY_;
    AMControl *ssaManipulatorY_;

    /// Z motor
    //SGMMAXvMotor *ssaManipulatorZ_;
    AMControl *ssaManipulatorZ_;

    /// rotation motor
    //SGMMAXvMotor *ssaManipulatorRot_;
    AMControl *ssaManipulatorRot_;

    /// enables or disables motor movement
    bool enableMotorMovement_;

    /// center and direction of rotation
    QVector3D centerOfRotation_;
    QVector3D directionOfRotation_;

    /// list of points used in camera calibration
    AMShapeData* calibrationPoints_[SAMPLEPOINTS];


    /// the shape to draw on, if enabled
    AMShapeData* drawOnShape_;

    /// true if drawing on shape is enabled
    bool drawOnShapeEnabled_;

    /// true if a drawOnShape_ is a valid shape
    bool drawOnShapeSelected_;

    /// the polygon currently being drawn
    QPolygonF currentPolygon_;

    /// markers used to show/set the location of the beam
    AMShapeData* beamMarkers_[3];

    /// shape used to show/set the sample plate location
    AMShapeData* samplePlateShape_;

    /// checks if sample plate is a valid shape
    bool samplePlateSelected_;

    /// shape used to check camera configuration
    AMShapeData* cameraConfigurationShape_;

    bool overrideMouseSelection_;







};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
