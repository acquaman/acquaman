/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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

/// \todo use MPL Eigen only
/// #define EIGEN_MPL2_ONLY
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
class AMAction3;

using namespace Eigen;

#define AMSAMPLECAMERA_DEBUG_OUTPUT 671001
#define AMSAMPLECAMERA_INCOMPLETE_BEAM_MODEL 671002
#define AMSAMPLECAMERA_FINISHRECTANGLE_NULL_SHAPE 671003
#define AMSAMPLECAMERA_ONESELECT_INVALID_INDEX 671004
#define AMSAMPLECAMERA_TWOSELECT_INVALID_INDEX 671005
#define AMSAMPLECAMERA_BEAMCALIBRATE_ERROR_DELETING_SHAPES 671006
#define AMSAMPLECAMERA_SETSAMPLEPLATE_BAD_INDEX 671007
#define AMSAMPLECAMERA_CREATESAMPLEPLATE_UNEXPECTED_NUMBER_OF_COORDINATES 671008
#define AMSAMPLECAMERA_CREATESAMPLEPLATE_TOO_FEW_POINTS 671009
#define AMSAMPLECAMERA_CONFIGUREROTATION_LENGTH_MISMATCH 671010
#define AMSAMPLECAMERA_ADDBEAMMARKER_NULL_MARKER 671011
#define AMSAMPLECAMERA_MOVEMOTORTO_FAILED_TO_MOVE 671012
#define AMSAMPLECAMERA_MOVEMOTORTO_CANNOT_MOVE_MOTORS 671013
#define AMSAMPLECAMERA_STOPMOTORS_CANNOT_STOP_MOTORS 671014
#define AMSAMPLECAMERA_REMOVESAMPLE_FAILED 671015
#define AMSAMPLECAMERA_REMOVESHAPEDATA_FAILED_NULL_SHAPE 671016
#define AMSAMPLECAMERA_REMOVESHAPEDATA_FAILED 671017
#define AMSAMPLECAMERA_SEVERE_STARTUP_DATABASE_ISSUES 671018
#define AMSAMPLECAMERA_FAILED_TO_STORE_DEFAULT_BEAM 671019
#define AMSAMPLECAMERA_ROTATION_AXIS_NOT_SPECIFIED 671020
#define AMSAMPLECAMERA_NULL_SUBSHAPE 671021
#define AMSAMPLECAMERA_GETPOINTONSHAPE_LINE_IS_ON_PLANE 671022
#define AMSAMPLECAMERA_GETPOINTONSHAPE_LINE_IS_PARALLEL_TO_PLANE 671023
#define AMSAMPLECAMERA_BEAMINTERSECTIONPOINT_RAY_DOES_NOT_MOVE_IN_Y 671024
#define AMSAMPLECAMERA_MOVEMOTORS_FAILURE 671025
#define AMSAMPLECAMERA_ROTATION_MATRIX_COUNT_MISMATCH 671026

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
	AMAngle *motorRotation() const;

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

	/// returns whether distortion is being used
	bool distortion();

	/// Returns whether or not the camera has been configured successfully
	bool cameraConfigured() const;
	/// Returns whether or not the rotation has been configured successfully
	bool rotationConfigured() const;
	/// Returns whether or not the sample plate has been configured successfully
	bool samplePlateConfigured() const;
	/// Returns whether or not the beam has been configured successfully
	bool beamConfigured() const;
	/// Returns true only if all of the configurations are true (camera, rotation, sample plate, beam)
	bool allConfigured() const;

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

	void setMotorX(double x);
	void setMotorY(double y);
	void setMotorZ(double z);
	void setMotorR(double r);

	/// toggles distortion on or off
	void toggleDistortion();

	void setDistortion(bool distortion);


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

	/// Allows external access so others can set the motor controls we'll use for the X motor
	void setSSAManipulatorX(AMControl *ssaManipulatorX);
	/// Allows external access so others can set the motor controls we'll use for the Y motor
	void setSSAManipulatorY(AMControl *ssaManipulatorY);
	/// Allows external access so others can set the motor controls we'll use for the Z motor
	void setSSAManipulatorZ(AMControl *ssaManipulatorZ);
	/// Allows external access so others can set the motor controls we'll use for the rotation motor
	void setSSAManipulatorRot(AMControl *ssaManipulatorRot);


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
	/// argument points is the series of points to fit a line to
	QVector<QVector3D> lineOfBestFit(const QList<QVector3D> &points) const;

	/// returns true if there is a shape to draw on, and it has been enabled
	bool drawOnShapeValid();
	/// returns true if the draw on shape option is enabled
	bool drawOnShapeEnabled();
	/// returns true if a shape to draw on has been selected
	bool drawOnShapeSelected();

	/// returns true if beam is not fully over sample area
	bool isBeamCutOff();

	/// returns true if a pair of samples occupy the same space
	bool samplesOverlap();



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
	/// This must be called before mouse based movement occurs
	/// so that the inital mouse position may be set, and movement
	/// may be tracked
	void setShapeVectors(QPointF);

	/// zooms all shapes
	void zoomAllShapes(QPointF);

	/// initializes the point to zoom wrt
	void setZoomPoint(QPointF);

	/// apply rotation to the rectangle, by moving mouse sideways
	void rotateRectangle(QPointF);

	/// apply zoom, by moving mouse up/down
	void zoomShape(QPointF);

	/// request to move to a particular sample
	void moveToSampleRequested(AMShapeData *shapeData);
	/// Returns an action (list of actions) to move to the requested sample
	AMAction3* createMoveToSampleAction(const AMSample *sample);

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

	/// sets a beam marker at the specified position
	void setBeamMarker(QPointF position, int index);
	/// updates the specified beam marker
	void updateBeamMarker(QPointF position, int index);

	/// calculates the trajectory of the beam, based on the beam calibration markers
	void beamCalibrate();

	void createBeamShape(int index);

	/// moves the specified beam shape
	void moveBeamShape(QPointF point, int index);
	/// sets the initial click position for moving the beam
	void beamMousePressed(QPointF point, int index);

	void setSamplePlate();
	void setSamplePlate(AMShapeData* samplePlate);
	void setSimpleSamplePlate(QVector3D base, QVector3D width, QVector3D height);
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
	/// move the sample plate under the mouse
	void moveSamplePlate(QPointF shift);

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

	/// Sets the cameraConfigured value to the boolean
	void setCameraConfigured(bool cameraConfigured);
	/// Sets the rotationConfigured value to the boolean
	void setRotationConfigured(bool rotationConfigured);
	/// Sets the samplePlateConfigured value to the boolean
	void setSamplePlateConfigured(bool samplePlateConfigured);
	/// Sets the beamConfigured value to the boolean
	void setBeamConfigured(bool beamConfigured);

	/// Returns whether debugging messages are currently to be sent to AMErrorMon
	bool debuggingSuppressed();

	/// Sets whether debugging messages should be sent to AMErrorMon
	void setDebuggingSuppressed(bool value);

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


	/// Sample Manipulator access
	double manipulatorX();
	double manipulatorY();
	double manipulatorZ();
	double manipulatorR();

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
	QVector3D rotateCoordinate(QVector3D coordinate, QVector3D center, QVector3D direction, AMAngle rotation) const;
	QVector3D rotateCoordinateByMatrix(QVector3D coordinate, QVector3D center, QVector3D direction, AMAngle rotation) const;


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
	QVector<QVector3D> findIntersectionShape(int index, bool *isFullyWithinSample = 0) const;
	QVector<QVector3D> findIntersectionShape(const AMShapeData* shape, bool boundIntersection = true, bool *isFullyWithinSample = 0) const;

	/// stops the beam from continuing through samples. Uses the shapes stored in
	/// intersectionShapes_
	void blockBeam();

	void setBeamCutOff(bool beamCutOff);

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
public:
	void insertItem(AMShapeData* item);
	/// checks to see if two QVector3D's are approximately equal, to the specified precision
	bool equalVectors(QVector3D a, QVector3D b, double precision = 0.001) const;

protected:
	/// removes an item from the shape list and returns it
	/// use this rather than taking manually - keeps the model updated
	AMShapeData* takeItem(int index);

	/// used to find the point to move the plate to, to position beneath the beam
	QVector3D beamIntersectionPoint(QVector3D samplePoint, bool findCorner = false);

	/// move the motors
	bool moveMotors(double x, double y, double z);

	bool moveMotors(double x, double y, double z, double rotation);


	QVector<QVector3D> findSamplePlateCoordinate(const QVector<QVector3D> coordinates, const QVector<QPointF> points, const QVector<double> rotations, const int numberOfPoints) const;
	MatrixXd constructSampleCoefficientMatrix(const QVector<QVector3D> vectors, const QVector<QVector3D> rotationX, const QVector<QVector3D> rotationY, const QVector<QVector3D> rotationZ, const int numberOfPoints) const;
	/// these three functions each generate a row of a general rotation matrix
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
	MatrixXd vectorToMatrix(QVector3D vector, MatrixXd matrix = MatrixXd(3,1)) const;
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
	AMAngle *motorRotation_;

	/// if true show distortion
	bool distortion_;

	/// intersections
	QVector<QPolygonF> intersections_;
	QVector<QVector<QVector3D> > intersectionShapes_;

	/// the position of the crosshair
	QPointF crosshair_;

	bool crosshairLocked_;


	/// used for transforming points to where they actually appear
	QSizeF viewSize_;
	QSizeF scaledSize_;

	/// vector for calculating shape movement
	QPointF currentVector_;
	/// 3D vector for calculating shape movement
	QVector3D oldVector_;

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

	/// overrides normal mouse commands. Stops text selection from deselecting current item
	bool overrideMouseSelection_;

	/// if true, operation moves to beam. If false, operation moves to crosshair.
	bool moveToBeam_;

	/// if true, move samples along the sample plate
	bool moveOnShape_;

	double videoTop_;

	AMAngle *oldRotation_;

	AMDeferredFunctionCall motorUpdateDeferredFunction_;

	/// if true, the beam is not fully within any sample's bounds
	bool beamCutOff_;

	/// Set to true if the camera configuration is successfully loaded
	bool cameraConfigured_;
	/// Set to true if the rotation configuration is successfully loaded
	bool rotationConfigured_;
	/// Set to true if the sample plate configuration is successfully loaded
	bool samplePlateConfigured_;
	/// Set to true if the beam configuration is successfully loaded
	bool beamConfigured_;
	/// Whether or not to output debugging messages
	bool debuggingSuppressed_;
};

#endif // AMSHAPEOVERLAYVIDEOWIDGETMODEL2_H
