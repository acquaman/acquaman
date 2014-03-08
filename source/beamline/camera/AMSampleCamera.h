#ifndef AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
#define AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H

#include <QGraphicsView>
#include <QWidget>
#include <QObject>
#include <QSizeF>
#include <QMap>
#include "beamline/camera/AMShapeData.h"
#include <QVector>
#include <QVector3D>

#include "beamline/camera/AMBeamConfiguration.h"
#include <QPair>
#include <complex>
#include <QGenericMatrix>

#include <Eigen/SVD>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/LU>

#include "util/AMDeferredFunctionCall.h"

#define SAMPLEPOINTS 6

class AMShapeData;
class QGraphicsRectItem;
class QVector3D;
class QPolygonF;
class AMCameraConfiguration;
class AMControl;
class AMCamera;
class AMSample;
class AMSamplePlate;
class AMRotationalOffset;
class AMAngle;

using namespace Eigen;

/// AMSampleCamera is a singleton class that provides the model for
/// an AMSampleCameraView.  It includes a collection of shapes that need
/// to be represented in the view, and also includes a camera model for
/// the display of the shapes and a beam model for positioning samples.

class AMSampleCamera: public QAbstractListModel
{
	Q_OBJECT
public:

	/// access to the AMSampleCamera
	static AMSampleCamera* set();

	/// Define axis names for convenience
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
	QVector3D motorCoordinate() const;
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

	/// viewSize and scaled_size used for computing coordinates, get values from AMSampleCameraGraphicsView
	QSizeF viewSize() const;
	QSizeF scaledSize() const;

	/// true if using cameraMatrix for transforms
	bool useCameraMatrix() const;

	/// the polygon currently being drawn
	QPolygonF currentPolygon() const;

	/// true if operation will move to beam, false if it will move to crosshair
	bool moveToBeam();

	bool moveOnShape();

	/// the sample plate screen shape
	QPolygonF samplePlate();

	/// true if using an offset for samples
	bool useSampleOffset();

	/// ------------------------------------------------------------------------------------------------


	/// other accessors
	QString currentName() const;
	QString name(int index) const;
	QString currentInfo() const;
	QString otherDataOne(int index) const;
    QString otherDataTwo(int index) const;
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

	/// the offset of the center of rotation.
	QVector3D rotationalOffset() const;

	double rotationalOffsetX() const;
	double rotationalOffsetY() const;
	double rotationalOffsetZ() const;

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
	/// sets the motor cooridinates using the current motor values
	void setMotorCoordinate();

	/// toggles distortion on or off
	void toggleDistortion();

	/// crosshair
	void setCrosshair(QPointF crosshair);
	void setCrosshairX(double x);
	void setCrosshairY(double y);

	void setCrosshairLocked(bool locked);

	/// view size and scaled size are used to
	/// determine the position of the camera image
	void setViewSize(QSizeF viewSize);

	void setScaledSize(QSizeF scaledSize, double videoTop);


	/// mutators for shape elements
    void setCurrentName(QString name);
	void setCurrentInfo(const QString &info);
    void setRotation(double rotation, int index = -1);
    void setTilt(double tilt, int index = -1);
    void setName(QString name, int index);
	void setOtherDataOne(const QString &data, int index);
    void setOtherDataTwo(QString data, int index);
    void setIdNumber(double number, int index);
    void setVisible(bool visible);
    void setVisible(bool visible, int index);


	/// sets whether to use camera matrix for transforms
	void setUseCameraMatrix(bool use);

	/// sets if mouse selection should be overridden (to prevent performing other actions)
	void setOverrideMouseSelection(bool overrideMouseSelection);

	/// sets the current index directly.  Also sets overrideMouseSelection to true
	void setCurrentShapeIndex(int index);

	/// set to true to move to beam.  Set to false to move to crosshair
	void setMoveToBeam(bool move);

	/// set to move parallel to the selected shape's plane (usually the sample plate shape)
	/// (rather than parallel to the camera view plane)
	void setMoveOnShape(bool moveOnShape);

	/// set whether to use the sampleOffset
	void setUseSampleOffset(bool useSampleOffset);

	/// set the offset of rotation (the centre of rotation at 0,0,0)
	void setRotationalOffset(const QVector3D &offset);
	void setRotationalOffsetX(const double &xCoordinate);
	void setRotationalOffsetY(const double &yCoordinate);
	void setRotationalOffsetZ(const double &zCoordinate);

	/// handles loading in an old sample plate
	void onSamplePlateLoaded(AMSamplePlate* plate);



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

	/// deletes the shape with the specified index
	void deleteShape(int index);




	/// list model functions
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;

	/// returns true if motor movement is enabled
	bool motorMovementEnabled() const;

	/// the list of sample shapes
	const QList<AMShapeData*> shapeList() const;

	/// if true mouse selection will not perform normal actions (but can put focus on text items)
	bool overrideMouseSelection() const;

	/// calculates the line of best fit for the given points
	/// the line is given by two points, the first is the centroid of
	/// all the points and the second is another point on the line
	QVector<QVector3D> lineOfBestFit(const QList<QVector3D> &points) const;



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

	void updateShape(AMShapeData* data);

	/// updates all the shapes
	void updateAllShapes();

	/// sets the shapes coordinate
	void setCoordinates(QVector3D, int);

	/// set coordinates with doubles rather than a QVector3D
	void setCoordinates(double x, double y, double z, int index = -1);

	/// places a grid (for distortion calibration)
	void placeGrid(const QPointF &position);

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
	void setSamplePlate(AMShapeData* samplePlate);
	/// Create the sample plate using selected points \todo
	void createSamplePlate(QVector<QVector3D> coordinates, QVector<QPointF> points, QVector<double> rotations, int numberOfPoints);

	/// saves the sample plate to the database
	void saveSamplePlate();

	void setCameraConfigurationShape();

	void configureRotation(const QVector<QVector3D> coordinates, const QVector<QPointF> points, const QVector<AMAngle *> rotations, const int numberOfPoints);
	void saveRotationalOffset();

	void moveSamplePlateTo(const QVector3D &coordinate);

	/// move the sample plate by a small amount, proportional to movement
	void moveSamplePlate(int movement);

	/// adds the beam marker, to mark the beam on the sample plate
	void addBeamMarker(int index);

	/// updates the view
	void updateView();

	/// moves the xyz motors to the specified location and rotation
	void moveMotorTo(QVector3D coordinate, double rotation);

	/// stop the motors
	void stopMotors();

	/// Takes an AMSample and add the related shapeData if that AMSample is not currently drawn
	void addSample(AMSample *sample);
        /// Takes an AMSample and removes the related shapeData if that AMSample is currently drawn
	void removeSample(AMSample *sample);

	/// deletes the specified shape.
	void removeShapeData(AMShapeData* shape);

	/// Loads the "default" beam configuration
	void loadDefaultBeam();
	/// Loads the "default" camera configuration
	void loadDefaultCamera();
	/// Loads the "default" sample plate configuration
	void loadDefaultSamplePlate();

	void setSamplePlateOffsetPlate();
	void setSamplePlateOffsetWafer();
	void setSamplePlateOffset(double offset);


signals:
	/// used to change the beam
	void beamChanged(QObject*);

	/// update coordinates
	void motorMoved();

	/// emitted upon a successful motor movement
	void moveSucceeded();

	/// emitted when shapeList_ is changed
	void shapesChanged();

	/// emitted when the current index changeds
	void currentIndexChanged(int);

	/// emitted when a shape's name changes
	void shapeNameChanged(QString newName);

	/// emitted when a shape has been drawn
	void shapeFinished();

	void otherDataOneChanged(QString data);

	void otherDataTwoChanged(QString data);

	void sampleShapeDeleted(AMShapeData *sampleShape);

	void cameraConfigurationChanged(AMCameraConfiguration*);

	void shapeDataChanged();

	void motorCoordinateChanged(QVector3D);
	void motorRotationChanged(double);

	void rotationalOffsetChanged(QVector3D);

	void enableMotorMovementChanged(bool enabled);
	void enableMotorTrackingChanged(bool enabled);

	void drawOnShapeEnabledChanged(bool enabled);

protected slots:
	/// tracks the motor location
	void motorTracking(double);

	/// checks to see if the motor is finished moving
	void motorsFinishedMoving();

	void onShapeDataChanged();

	void saveBeam();
	void saveCamera();

protected:

	/// Constructor
	explicit AMSampleCamera(QObject *parent = 0);


	/// Manipulations


	/// shifts all coordinates by a given amount
	void shiftCoordinates(QVector3D shift, int index);
	void shiftCoordinates(QVector3D shift, AMShapeData* shape);

	/// apply motor rotation to the shape
	void applyMotorRotation(int index, double rotation);
	QVector<QVector3D>  applyMotorRotation(AMShapeData* shape, double rotation) const;
	/// apply the rotation in the given direction by the given angle with the given centre to the give shape
	AMShapeData* applySpecifiedRotation(const AMShapeData *shape, QVector3D direction, QVector3D centre, double angle) const;

	/// apply the rotation in the given direction by the given angle to the given shape
	AMShapeData* applySpecifiedRotation(const AMShapeData* shape, QVector3D direction, double angle) const;

	/// convenience function for above apply specified rotation.
	/// applies axis rotation to the shape, in direction XAXIS, YAXIS, or  ZAXIS, by the amount given in shape.
	AMShapeData* applySpecifiedRotation(const AMShapeData* shape, AxisDirection direction) const;

	/// general rotation of a point about a point along the direction
	QVector3D rotateCoordinate(QVector3D coordinate, QVector3D center, QVector3D direction, double rotation) const;
	QVector3D rotateCoordinateByMatrix(QVector3D coordinate, QVector3D center, QVector3D direction, double rotation) const;


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
	QVector<QVector3D> findIntersectionShape(const AMShapeData* shape, bool boundIntersection = true) const;

	/// converts the intersection shape to a shape on the screen
	QPolygonF intersectionScreenShape(QVector<QVector3D>) const;

	/// finds the center coordinate of the given index
	QVector3D centerCoordinate(int index) const;

	/// finds the dot product of two vectors
	double dot(QVector3D,QVector3D) const;

	/// returns the unit vector in the direction of the last line in the shape.
	/// for a rectangle this is the "upward" direction
	QVector3D getHeightNormal(const AMShapeData* shape) const;
	/// returns the unit vector in the direction of the first line in the shapeAMShapeData
	/// for a rectangle this is the "right" direction
	QVector3D getWidthNormal(const AMShapeData* shape) const;
	/// returns the vector normal to heightVector and widthVector
	QVector3D getNormal(QVector3D heightVector, QVector3D widthVector) const;
	/// returns the vector normal to the given shape
	QVector3D getNormal(const AMShapeData* shape) const;
	/// returns the 3D point that lies under position and on the plane defined by the drawOnShape_ shape
	QVector3D getPointOnShape(const QPointF &position, const QVector3D &normal, bool useOffset = false) const;
	/// returns the 3D point that lies under position and on the plane defined by shape
	QVector3D getPointOnShape(const AMShapeData* shape, const QPointF &position, const QVector3D &normal, bool useOffset = false) const;
	QVector3D getPointOnShape(const AMShapeData* shape, const QPointF &position, bool useOffset = false) const;

	/// returns the unit vector in the direction of the top of the screen to the bottom
	QVector3D downVector() const;
	/// returns the unit vector in the direction from the left of the screen to the right
	QVector3D rightVector() const;

	/// inserts an item into the shape list - use this rather than inserting
	/// manually into the list - keeps the model updated
	void insertItem(AMShapeData* item);

	/// removes an item from the shape list - use this rather than
	/// removing items manually - keeps the model updated
//	void removeItem(int index);

	/// removes an item from the shape list and returns it
	/// use this rather than taking manually - keeps the model updated
	AMShapeData* takeItem(int index);

	/// used to find the point to move the plate to, to position beneath the beam
	QVector3D beamIntersectionPoint(QVector3D samplePoint);

	/// move the motors
	bool moveMotors(double x, double y, double z);

	bool moveMotors(double x, double y, double z, double rotation);


	QVector<QVector3D> findSamplePlateCoordinate(const QVector<QVector3D> coordinates, const QVector<QPointF> points, const QVector<double> rotations, const int numberOfPoints) const;
	MatrixXd constructSampleCoefficientMatrix(const QVector<QVector3D> vectors, const QVector<QVector3D> rotationX, const QVector<QVector3D> rotationY, const QVector<QVector3D> rotationZ, const int numberOfPoints) const;
	/// these three functions each generate a row of a rotation matrix
	QVector3D findRotationMatrixXRow(QVector3D rotationVector, double angleOfRotation) const;
	QVector3D findRotationMatrixYRow(QVector3D rotationVector, double angleOfRotation) const;
	QVector3D findRotationMatrixZRow(QVector3D rotationVector, double angleOfRotation) const;
	/// construct a general rotation matrix
	MatrixXd constructRotationMatrix(QVector3D rotationVector, double angleOfRotation) const;

	/// returns two vectors describing the line that lies along point
	/// using the provided coordinates for depth
	/// the result will be a point along the line and a unit vector in the direction of the line
	QPair<QVector3D,QVector3D> findScreenVector(QPointF point, QVector3D referenceCoordinate, QVector3D optionalReference = QVector3D(0,0,0));
	MatrixXd constructCentreOfRotationMatrix(const QVector<AMAngle *> &rotations, const QVector<QVector3D> &vectors, const int &numberOfPoints);
	MatrixXd constructCentreOfRotationCoordinateMatrix(const QVector<QVector3D> &coordinates, QVector<QVector3D> bases, const int &numberOfPoints);
	MatrixXd solveCentreOfRotationMatrix(MatrixXd coeffMatrix, MatrixXd coordinateMatrix);
	/// computes the SVD least squares solution
	MatrixXd computeSVDLeastSquares(MatrixXd leftHandSide, MatrixXd rightHandSide) const;
	/// computes the SVD homogenous solution for the largest singular value
	MatrixXd computeSVDHomogenous(MatrixXd leftHandSide) const;

	/// convert a QVector3D to a 3x1 matrix
	MatrixXd vectorToMatrix(QVector3D vector) const;
	/// convert a 3 element matrix to a QVector3D
	QVector3D matrixToVector(MatrixXd matrix) const;

	/// prints out a matrix using qDebug, according to rows and columns
	void debugPrintMatrix(const MatrixXd matrix) const;


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
	AMControl *ssaManipulatorX_;

	/// Y motor
	AMControl *ssaManipulatorY_;

	/// Z motor
	AMControl *ssaManipulatorZ_;

	/// rotation motor
	AMControl *ssaManipulatorRot_;

	/// enables or disables motor movement
	bool enableMotorMovement_;

	/// center and direction of rotation
	QVector3D centerOfRotation_;
	AMRotationalOffset *rotationalOffset_;
	QVector3D directionOfRotation_;

	/// list of points used in camera calibration
	AMShapeData* calibrationPoints_[SAMPLEPOINTS];


	/// the shape to draw on, if enabled
	AMShapeData* drawOnShape_;

	/// true if drawing on shape is enabled
	bool drawOnShapeEnabled_;

        /// true if drawOnShape_ is a valid shape
	bool drawOnShapeSelected_;

	/// the offset from the shape to draw on
	/// i.e. draw on a parallel plane this distance from the shape
	double sampleOffset_;
	/// true if using the sampleOffset
	bool useSampleOffset_;

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

	/// overrides normal mouse commands; stops text selection from deselecting current item
	bool overrideMouseSelection_;

	/// if true, operation moves to beam. If false, operation moves to crosshair.
	bool moveToBeam_;

	bool moveOnShape_;

	double videoTop_;

	AMAngle *oldRotation_;

	AMDeferredFunctionCall motorUpdateDeferredFunction_;







};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
