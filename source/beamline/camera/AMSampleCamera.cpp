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


#include "AMSampleCamera.h"

#include <math.h>

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QMediaObject>
#include <QGraphicsVideoItem>
#endif

#include "AMQEvents.h"

#include <QGraphicsRectItem>
#include <QVector3D>
#include <QDebug>
#include <QVector4D>
#include <QApplication>

#include "beamline/camera/AMCameraConfiguration.h"
#include "beamline/camera/AMCamera.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "source/beamline/AMBeamline.h"
#include "source/beamline/camera/AMAngle.h"
#include "beamline/camera/AMRotationalOffset.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSamplePre2013.h"
#include "dataman/AMSample.h"
#include "dataman/AMSamplePlate.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMMAXvMotor.h"
#include "util/AMErrorMonitor.h"
#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMRIGHT 2
#define BOTTOMLEFT 3
#define TOPCLOSE 4
#define RECTANGLE_POINTS 5

using namespace Eigen;

AMSampleCamera* AMSampleCamera::set_;

/// provides access to the AMSampleCamera
AMSampleCamera* AMSampleCamera::set()
{
	if(!set_)
	{
		set_ = new AMSampleCamera();
	}
	return set_;

}


/// returns the highest index of shapes in shapeList
int AMSampleCamera::shapeListLength() const
{
	return index_;
}



/// returns the current index
int AMSampleCamera::currentIndex() const
{
	return currentIndex_;
}

/// returns the cameraConfiguration
AMCameraConfiguration *AMSampleCamera::cameraConfiguration() const
{
	return camera_->cameraConfiguration();
}

/// returns the beam configuration
AMBeamConfiguration *AMSampleCamera::beamConfiguration() const
{
	return beamModel_;
}

/// returns the group rectangle in screen coordinates
QPolygonF AMSampleCamera::groupRectangle() const
{
	return screenShape(groupRectangle_);
}

QVector3D AMSampleCamera::motorCoordinate() const
{
	return motorCoordinate_;
}

/// returns motor x coordinate
double AMSampleCamera::motorX() const
{
	return motorCoordinate().x();
}

/// returns motor y coordinate
double AMSampleCamera::motorY() const
{
	return motorCoordinate().y();
}

/// returns motor z coordinate
double AMSampleCamera::motorZ() const
{
	return motorCoordinate().z();
}

/// returns the motor rotation
AMAngle *AMSampleCamera::motorRotation() const
{
	return motorRotation_;
}

/// returns the intersections
QVector<QPolygonF> AMSampleCamera::intersections() const
{
	return intersections_;
}

/// returns the crosshair point
QPointF AMSampleCamera::crosshair() const
{
	return crosshair_;
}

/// returns the crosshair x value
double AMSampleCamera::crosshairX() const
{
	return crosshair_.x();
}

/// returns the crosshair y value
double AMSampleCamera::crosshairY() const
{
	return crosshair_.y();
}

/// is the crossHair locked?
bool AMSampleCamera::crosshairLocked() const
{
	return crosshairLocked_;
}

/// returns the view size of the video widget
QSizeF AMSampleCamera::viewSize() const
{
	return camera_->viewSize();
}

/// returns the scaled size of the video widget
QSizeF AMSampleCamera::scaledSize() const
{
	return camera_->scaledSize();
}

/// is the cameraMatrix being used?
bool AMSampleCamera::useCameraMatrix() const
{
	return camera_->useCameraMatrix();
}

/// The polyon currently being drawn
QPolygonF AMSampleCamera::currentPolygon() const
{
	return screenShape(currentPolygon_);
}

bool AMSampleCamera::moveToBeam()
{
	return moveToBeam_;
}

bool AMSampleCamera::moveOnShape()
{
	return moveOnShape_;
}

QPolygonF AMSampleCamera::samplePlate()
{
	if(samplePlateSelected_)
		return screenShape(subShape(samplePlateShape_));
	else return QPolygonF();

}

bool AMSampleCamera::useSampleOffset()
{
	return useSampleOffset_;
}

/// return the current name
QString AMSampleCamera::currentName() const
{
	return name(currentIndex_);
}

/// returns the name of the specified index
QString AMSampleCamera::name(int index) const
{
	if(isValid(index))
	{
		return shapeList_[index]->name();
	}
	else return "";
}

/// return the current info
QString AMSampleCamera::currentInfo() const
{
	return otherDataOne(currentIndex_);
}

/// return the data of the specified index
QString AMSampleCamera::otherDataOne(int index) const
{
	if(isValid(index))
	{
		return shapeList_[index]->otherDataFieldOne();
	}
	else
		return "";
}

QString AMSampleCamera::otherDataTwo(int index) const
{
	if(isValid(index))
	{
		return shapeList_[index]->otherDataFieldTwo();
	}
	else
	{
		return "";
	}
}

/// returns the idNumber of the specified index
double AMSampleCamera::idNumber(int index) const
{
	if(isValid(index))
	{
		return shapeList_[index]->idNumber();
	}
	else
		return 0;
}

bool AMSampleCamera::visible() const
{
	return visible(currentIndex_);
}

bool AMSampleCamera::visible(int index) const
{
	return shapeList_.at(index)->visible();
}

/// returns the rotation of the given index
double AMSampleCamera::rotation(int index) const
{
	if(isValid(index))
		return shapeList_[index]->rotation();
	else return 0;
}

/// returns the rotation of the current index
double AMSampleCamera::rotation() const
{
	return rotation(currentIndex_);
}

/// returns the tilt for the given index
double AMSampleCamera::tilt(int index) const
{
	if(isValid(index))
		return shapeList_[index]->tilt();
	else return 0;
}

/// returns the tilt for the current index
double AMSampleCamera::tilt() const
{
	return tilt(currentIndex_);
}

/// returns a pointer to the AMShapeData with  index, by default current_
AMShapeData *AMSampleCamera::currentShape(int index) const
{
	if(isValid(index))
		return shapeList_[index];
	else return 0;
}

AMShapeData *AMSampleCamera::currentShape() const
{
	return currentShape(currentIndex_);
}

int AMSampleCamera::indexOfShape(AMShapeData *shape) const
{
	return shapeList_.indexOf(shape);
}

/// returns the shape in screen coordinates
QPolygonF AMSampleCamera::shape(int index) const
{
	QPolygonF shape = *shapeList_[index]->shape();

	return screenShape(shape);

}

/// returns the coordinate of index
QVector3D AMSampleCamera::coordinate(int index) const
{
	return centerCoordinate(index);
}

/// calls coordinate with index equal to current_
QVector3D AMSampleCamera::currentCoordinate() const
{
	return coordinate(currentIndex_);
}

/// The offset of the centre of rotation from the motor coordinate
QVector3D AMSampleCamera::rotationalOffset() const
{
	return rotationalOffset_->rotationalOffset();
}

double AMSampleCamera::rotationalOffsetX() const
{
	return rotationalOffset().x();
}

double AMSampleCamera::rotationalOffsetY() const
{
	return rotationalOffset().y();
}

double AMSampleCamera::rotationalOffsetZ() const
{
	return rotationalOffset().z();
}

bool AMSampleCamera::distortion()
{
	return distortion_;
}

bool AMSampleCamera::cameraConfigured() const{
	return cameraConfigured_;
}

bool AMSampleCamera::rotationConfigured() const{
	return rotationConfigured_;
}

bool AMSampleCamera::samplePlateConfigured() const{
	return samplePlateConfigured_;
}

bool AMSampleCamera::beamConfigured() const{
	return beamConfigured_;
}

bool AMSampleCamera::allConfigured() const{
	return cameraConfigured_ && rotationConfigured_ && samplePlateConfigured_ && beamConfigured_;
}

/// set the current index
void AMSampleCamera::setCurrentIndex(int current)
{
	currentIndex_ = current;
	emit currentIndexChanged(currentIndex_);
}

/// changes the current camera model
void AMSampleCamera::setCameraModel(AMCameraConfiguration *model)
{
	camera_->setCameraConfiguration(model);
	updateAllShapes();
}

/// sets the beam model to the given model
void AMSampleCamera::setBeamModel(AMBeamConfiguration *model)
{
	if(beamModel_ != model)
	{
		beamModel_ = model;
		emit beamChanged(beamModel_);
	}
}

/// sets the motor coordinate to xyzr
void AMSampleCamera::setMotorCoordinate(double x, double y, double z, double r)
{
	motorMovement(x,y,z,r);
	setMotorX(x);
	setMotorY(y);
	setMotorZ(z);
	setMotorR(r);
}

/// sets the motor coordinate to xyzr (the current motor value)
void AMSampleCamera::setMotorCoordinate()
{
	double x = manipulatorX();
	double y = manipulatorY();
	double z = manipulatorZ();
	double r = manipulatorR();
	setMotorCoordinate(x,y,z,r);

	// update the sample plate
	AMSamplePlate* currentPlate = AMBeamline::bl()->samplePlate();
	if(currentPlate)
	{
		currentPlate->setPlatePosition(motorCoordinate());
		currentPlate->setPlateRotation(motorRotation()->degrees());
	}
}

void AMSampleCamera::setMotorX(double x)
{
	motorCoordinate_.setX(x);
}

void AMSampleCamera::setMotorY(double y)
{
	motorCoordinate_.setY(y);
}

void AMSampleCamera::setMotorZ(double z)
{
	motorCoordinate_.setZ(z);
}

void AMSampleCamera::setMotorR(double r)
{
	motorRotation_->setDegrees(r);
}


/// toggles distortion on or off
void AMSampleCamera::toggleDistortion()
{

	setDistortion(!distortion());

	if(distortion() && debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Distortion on") );
	else if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Distortion off") );
	updateAllShapes();
}

void AMSampleCamera::setDistortion(bool distortion)
{
	distortion_ = distortion;
}



/// set the crosshair point
void AMSampleCamera::setCrosshair(QPointF crosshair)
{
	crosshair_ = crosshair;
}

/// set the crosshair x value
void AMSampleCamera::setCrosshairX(double x)
{
	crosshair_.setX(x);
}

/// set the crosshair y value
void AMSampleCamera::setCrosshairY(double y)
{
	crosshair_.setY(y);
}

/// set whether the crosshair is locked
void AMSampleCamera::setCrosshairLocked(bool locked)
{
	crosshairLocked_ = locked;
}

/// set the view size
void AMSampleCamera::setViewSize(QSizeF viewSize)
{
	camera_->setViewSize(viewSize);
}

/// set the scaled size
void AMSampleCamera::setScaledSize(QSizeF scaledSize, double videoTop)
{
	camera_->setScaledSize(scaledSize);
	videoTop_ = videoTop;
}

/// set the current name
void AMSampleCamera::setCurrentName(QString name)
{
	setName(name,currentIndex_);
}

/// set the current info
void AMSampleCamera::setCurrentInfo(const QString &info)
{
	setOtherDataOne(info,currentIndex_);
}

/// sets the rotation for the given index
void AMSampleCamera::setRotation(double rotation, int index)
{
	if(-1 == index)
	{
		index = currentIndex_;
	}
	shapeList_[index]->setRotation(rotation);
}

/// sets the tilt for the given index
void AMSampleCamera::setTilt(double tilt, int index)
{
	if(-1 == index)
	{
		index = currentIndex_;
	}
	shapeList_[index]->setTilt(tilt);
}

/// sets the name of the specified index
void AMSampleCamera::setName(QString name, int index)
{
	if(isValid(index))
	{
		shapeList_[index]->setName(name);
	}
}

/// sets the data of the specifed index
void AMSampleCamera::setOtherDataOne(const QString &data, int index)
{
	if(isValid(index))
	{
		shapeList_[index]->setOtherDataFieldOne(data);
	}
}

void AMSampleCamera::setOtherDataTwo(QString data, int index)
{
	if(isValid(index))
		shapeList_[index]->setOtherDataFieldTwo(data);
}

/// sets the id number of the specifed index
void AMSampleCamera::setIdNumber(double number, int index)
{
	if(isValid(index))
	{
		shapeList_[index]->setIdNumber(number);
	}
}

void AMSampleCamera::setVisible(bool visible)
{
	setVisible(visible,currentIndex_);
}

void AMSampleCamera::setVisible(bool visible, int index)
{
	shapeList_[index]->setVisible(visible);
}

/// sets whether to use camera matrix for transforms
void AMSampleCamera::setUseCameraMatrix(bool use)
{
	camera_->setUseCameraMatrix(use);
}

void AMSampleCamera::setOverrideMouseSelection(bool overrideMouseSelection)
{
	overrideMouseSelection_ = overrideMouseSelection;
}

void AMSampleCamera::setCurrentShapeIndex(int index)
{
	setOverrideMouseSelection(true);
	setCurrentIndex(index);
}

void AMSampleCamera::setMoveToBeam(bool move)
{
	moveToBeam_ = move;
}

void AMSampleCamera::setMoveOnShape(bool moveOnShape)
{
	moveOnShape_ = moveOnShape;
}

void AMSampleCamera::setUseSampleOffset(bool useSampleOffset)
{
	useSampleOffset_ = useSampleOffset;
}

void AMSampleCamera::setRotationalOffset(const QVector3D &offset)
{
	rotationalOffset_->setRotationalOffset(offset);
	centerOfRotation_ = motorCoordinate() + rotationalOffset();
	emit rotationalOffsetChanged(rotationalOffset());
}

void AMSampleCamera::setRotationalOffsetX(const double &xCoordinate)
{
	QVector3D newOffset(xCoordinate,rotationalOffsetY(), rotationalOffsetZ());
	setRotationalOffset(newOffset);
}

void AMSampleCamera::setRotationalOffsetY(const double &yCoordinate)
{
	QVector3D newOffset(rotationalOffsetX(),yCoordinate, rotationalOffsetZ());
	setRotationalOffset(newOffset);
}

void AMSampleCamera::setRotationalOffsetZ(const double &zCoordinate)
{
	QVector3D newOffset(rotationalOffsetX(),rotationalOffsetY(), zCoordinate);
	setRotationalOffset(newOffset);
}




void AMSampleCamera::onSamplePlateLoaded(AMSamplePlate* plate)
{
	/// sample plate has been loaded out of the database, must correct the positions
	// get the shift and rotation
	QVector3D platePosition = plate->platePosition();

	QVector3D currentPosition = motorCoordinate();
	QVector3D shiftAmount = currentPosition - platePosition;
	oldRotation_->setDegrees(plate->plateRotation());
	AMAngle rotation (motorRotation()->degrees(), AMAngle::DEG);
	foreach(AMShapeData* shape, shapeList_)
	{
		shape->shift(shiftAmount);
		shape->setCoordinateShape(applyMotorRotation(shape, rotation.radians()));
	}
	oldRotation_->setAngle(rotation);
	emit motorCoordinateChanged(motorCoordinate());
	emit motorRotationChanged(motorRotation()->degrees());
}

void AMSampleCamera::setSSAManipulatorX(AMControl *ssaManipulatorX){
	ssaManipulatorX_ = ssaManipulatorX;
	if(ssaManipulatorX_)
		connect(ssaManipulatorX_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
}

void AMSampleCamera::setSSAManipulatorY(AMControl *ssaManipulatorY){
	ssaManipulatorY_ = ssaManipulatorY;
	if(ssaManipulatorY_)
		connect(ssaManipulatorY_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
}

void AMSampleCamera::setSSAManipulatorZ(AMControl *ssaManipulatorZ){
	ssaManipulatorZ_ = ssaManipulatorZ;
	if(ssaManipulatorZ_)
		connect(ssaManipulatorZ_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
}

void AMSampleCamera::setSSAManipulatorRot(AMControl *ssaManipulatorRot){
	ssaManipulatorRot_ = ssaManipulatorRot;
	if(ssaManipulatorRot_){
		motorRotation_->setDegrees(ssaManipulatorRot_->value());
		connect(ssaManipulatorRot_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
	}
}

/// checks if an index is valid
bool AMSampleCamera::isValid(int index) const
{
	return (index <= index_ && index >= 0);
}

/// checks to see if a shape is backwards
bool AMSampleCamera::isBackwards(int index) const
{
	if(isValid(index))
		return shapeList_[index]->backwards();
	else return 0;
}

bool AMSampleCamera::isBackwards() const
{
	return isBackwards(currentIndex_);
}

/// figures out camera parameters given a set of points and coordinates
/// the points should be based on top left corner = 0,0
/// and bottom right corner = 1,1
/// coordinates should be 6 unique coordinates
/// which define a shape
/// (no three of the points should lie upon the same line)
void AMSampleCamera::findCamera(QPointF points [], QVector3D coordinates[])
{

	/// center the image
	QPointF point[SAMPLEPOINTS];
	for(int i = 0; i < SAMPLEPOINTS; i++)
	{
		point[i].setX((points[i].x()-0.5));
		point[i].setY((points[i].y()-0.5));
		/*
		point[i] = undistortPoint(point[i]);
		*/
	}
	/// auto draw shapes at the points, for convenience
	/// these should appear at "points" if the calibration was successful
	deleteCalibrationPoints();
	AMShapeData* shapes[SAMPLEPOINTS];
	for(int i = 0; i < SAMPLEPOINTS; i++)
	{
		shapes[i] = new AMShapeData();
		QVector<QVector3D> shapeCoordinates;
		shapeCoordinates<<QVector3D(-0.2,0.2,0)<<QVector3D(0.2,0.2,0)<<QVector3D(0.2,-0.2,0)<<QVector3D(-0.2,-0.2,0)<<QVector3D(-0.2,0.2,0);
		shapes[i]->setCoordinateShape(shapeCoordinates);
		shapes[i]->shiftTo(coordinates[i]);
		shapes[i]->setTilt(0);
		shapes[i]->setRotation(0);
		shapes[i]->setYAxisRotation(0);
		QString suffix;
		if(i == 1)
			suffix = "st";
		else if (i == 2)
			suffix = "nd";
		else if (i == 3)
			suffix = "rd";
		else
			suffix = "th";
		shapes[i]->setName(QString::number(i)+ suffix + " Calibration point");
		insertItem(shapes[i]);
		calibrationPoints_[i] = shapeList_[index_];
		updateShape(index_);
		setCurrentIndex(index_);
	}

	/// find the camera parameters
	camera_->getTransforms(point,coordinates);

	camera_->setCalibrationRun(true);

	camera_->cameraConfiguration()->storeToDb(AMDatabase::database("SGMPublic"));

}

/// finds all the intersections, returns false if there is no valid beam
bool AMSampleCamera::findIntersections()
{
	/// This finds all the intersections of the beam with the samples
	/// and determines how to draw it.


	/// Cannot find the beam with an incomplete beam model
	if(beamModel_->positionOne().isEmpty() || beamModel_->positionTwo().isEmpty())
	{
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_INCOMPLETE_BEAM_MODEL, QString("Cannot find beam, the beam model is incomplete.") );
		return false;
	}
	intersections_.clear();
	intersectionShapes_.clear();
	QList<AMSample*> allIntersectedSamples;
	bool isFullyWithinSample;
	bool beamInSample = false;

	/// for each shape, find if it intersects the beam, and if it does,
	/// determine the interseection shape
	for(int i = 0; i <= index_; i++)
	{
		QVector<QVector3D> intersectionShape = findIntersectionShape(i,&isFullyWithinSample);
		if(!intersectionShape.isEmpty()){
			if(AMBeamline::bl()->samplePlate())
				allIntersectedSamples.append(AMBeamline::bl()->samplePlate()->sampleFromShape(shapeList_.at(i)));
			intersections_<<intersectionScreenShape(intersectionShape);
			intersectionShapes_<<intersectionShape;
		}
		beamInSample |= isFullyWithinSample;
	}

	/// set beamCutOff to false if no samples fully contain the beam
	setBeamCutOff(!beamInSample);

	AMBeamline::bl()->setCurrentSamples(allIntersectedSamples);

	/// Find the intersection with the sample plate
	if(samplePlateSelected_)
	{
		QVector<QVector3D> intersectionShape = findIntersectionShape(samplePlateShape_, false);
		if(!intersectionShape.isEmpty())
		{
			intersections_<<intersectionScreenShape(intersectionShape);
			intersectionShapes_<<intersectionShape;
		}
	}

	/// fix the intersections so that the beam is stopped when it hits something, does not go through it.
	blockBeam();

	return true;
}

/// deletes the specifed shape
void AMSampleCamera::deleteShape(int index)
{
	if(isValid(index))
	{
		AMShapeData* shape = takeItem(index);
		shape->deleteLater();
	}
}

int AMSampleCamera::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return shapeList_.count();
}

QVariant AMSampleCamera::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return QVariant();
	switch(role)
	{
	case Qt::DisplayRole:
		return shapeList_.at(index.row())->name();
	default:
		return QVariant();
	}
}

bool AMSampleCamera::motorMovementEnabled() const
{
	return enableMotorMovement_;
}

const QList<AMShapeData *> AMSampleCamera::shapeList() const
{
	return shapeList_;
}

bool AMSampleCamera::overrideMouseSelection() const
{
	return overrideMouseSelection_;
}

/// solves for the line of best fit for a selection of points using
/// svd decomposition to get the least squares solution
QVector<QVector3D> AMSampleCamera::lineOfBestFit(const QList<QVector3D> &points) const
{
	int dimension = 3;
	// first find the centroid
	QVector3D centre(0,0,0);
	foreach(QVector3D point, points)
	{
		centre += point;
	}
	centre /= points.count();

	//	generate the distance matrix
	MatrixXd mMatrix(points.count(), dimension);
	for(int i = 0; i < points.count(); i++)
	{
		QVector3D point = points.at(i) - centre;
		mMatrix(i,0) = point.x();
		mMatrix(i,1) = point.y();
		mMatrix(i,2) = point.z();
	}


	// solution is first column of V matrix - solution for maximum singular value of m
	MatrixXd solution = computeSVDHomogenous(mMatrix);


	QVector3D unitVector(0,0,0);
	if(solution.rows() == 3)
	{
		unitVector.setX(solution(0,0));
		unitVector.setY(solution(1,0));
		unitVector.setZ(solution(2,0));
	}

	double t = 1;

	QVector3D linePoints;

	linePoints = centre+unitVector*t;

	QVector<QVector3D> line;
	line<<centre<<linePoints;

	return line;


}

bool AMSampleCamera::drawOnShapeValid()
{
	return drawOnShapeEnabled() && drawOnShapeSelected() && (drawOnShape_ != NULL);
}

bool AMSampleCamera::drawOnShapeEnabled()
{
	return drawOnShapeEnabled_;
}

bool AMSampleCamera::drawOnShapeSelected()
{
	return drawOnShapeSelected_;
}


/// creates a new rectangle, and initializes its data
void AMSampleCamera::startRectangle(QPointF position)
{

	position = undistortPoint(position);
	QVector<QVector3D> newShape;
	QVector3D coordinate[RECTANGLE_POINTS];
	double depth = camera_->focalLength();
	/// if drawing on shape, the point to start at lies
	/// on the shape to draw on.
	if(drawOnShapeValid())
	{
		QVector3D heightNormal = getHeightNormal(drawOnShape_);
		QVector3D widthNormal = getWidthNormal(drawOnShape_);
		QVector3D normal = getNormal(heightNormal, widthNormal);
		QVector3D rightNormal = getNormal(normal,heightNormal);
		QVector3D startPoint = getPointOnShape(position, normal, useSampleOffset());
		coordinate[TOPLEFT] = startPoint;
		coordinate[TOPRIGHT] = startPoint + 0.01*rightNormal;
		coordinate[BOTTOMRIGHT] =  startPoint + 0.01*rightNormal - 0.01*heightNormal;
		coordinate[BOTTOMLEFT] = startPoint - 0.01*heightNormal;
		coordinate[TOPCLOSE] = coordinate[TOPLEFT];

	}
	else
	{
		QVector3D start = camera_->transform2Dto3D(position,depth);
		QVector3D down = 0.001*downVector();
		QVector3D right = 0.001*rightVector();
		coordinate[TOPLEFT] = start;
		coordinate[TOPRIGHT] = start + right;
		coordinate[BOTTOMRIGHT] = start + right + down;
		coordinate[BOTTOMLEFT] = start + down;
		coordinate[TOPCLOSE] = start;
	}
	for(int i = 0; i<RECTANGLE_POINTS; i++)
	{
		newShape<<coordinate[i];
	}
	AMShapeData* newShapeData = new AMShapeData();

	newShapeData->setName("Shape " + QString::number(index_+1));
	newShapeData->setIdNumber((index_+1) * 13);
	newShapeData->setRotation(0);
	newShapeData->setTilt(0);
	newShapeData->setYAxisRotation(0);

	newShapeData->setCoordinateShape(newShape);
	insertItem(newShapeData);

	setCurrentIndex(index_);
}

/// Changes the bottom right corner of the current rectangle
void AMSampleCamera::finishRectangle(QPointF position)
{
	if(!isValid(currentIndex_))return;
	position = undistortPoint(position);
	QVector3D topLeft = shapeList_[currentIndex_]->coordinate(TOPLEFT);
	QVector3D bottomRight = camera_->transform2Dto3D(position,depth(topLeft));
	QVector3D topRight;
	QVector3D bottomLeft;
	if(drawOnShapeValid())
	{
		bottomRight = getPointOnShape(position,getNormal(drawOnShape_), useSampleOffset());
		topRight = getWidthNormal(drawOnShape_);
		bottomLeft = QVector3D::normal(topRight,getNormal(drawOnShape_));
	}
	else
	{
		topRight = rightVector();
		bottomLeft = downVector();
	}
	topRight.normalize();
	bottomLeft.normalize();
	QVector3D shift = bottomRight - topLeft;
	if(topRight.isNull() && bottomLeft.isNull())
	{
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_FINISHRECTANGLE_NULL_SHAPE, QString("Null shape in finishRectangle.") );
	}
	else if(topRight.isNull())
		topRight = QVector3D::normal(QVector3D::normal(bottomLeft,shift),bottomLeft);
	else if(bottomLeft.isNull())
		bottomLeft = QVector3D::normal(QVector3D::normal(topRight,shift),topRight);


	double topRightlength = (dot(topRight,shift));
	topRight = topRight*topRightlength;

	double topLeftlength = (dot(bottomLeft,shift));
	bottomLeft = bottomLeft*topLeftlength;


	shapeList_[currentIndex_]->setCoordinate(topLeft + topRight,TOPRIGHT);
	shapeList_[currentIndex_]->setCoordinate(topLeft+topRight+bottomLeft,BOTTOMRIGHT);
	shapeList_[currentIndex_]->setCoordinate(topLeft + bottomLeft,BOTTOMLEFT);
	updateShape(currentIndex_);
	emit shapeFinished();
	setOverrideMouseSelection(false);

}

/// deletes a rectangle from the list
void AMSampleCamera::deleteRectangle(QPointF position)
{
	bool deleted = false;
	for(int i = 0; i < index_ + 1; i++)
	{
		deleted = false;
		if(contains(position,i))
		{
			AMShapeData* polygon = takeItem(i);
			for(int j = 0; j < SAMPLEPOINTS; j++)
			{
				if(polygon == calibrationPoints_[j])
				{
					calibrationPoints_[j]->deleteLater();
					calibrationPoints_[j] = 0;
					j = SAMPLEPOINTS;
					deleted = true;

				}
				if(j < 3)
				{
					if(polygon == beamMarkers_[j])
					{
						beamMarkers_[j]->deleteLater();
						beamMarkers_[j] = 0;
						j = SAMPLEPOINTS;
						deleted = true;
					}
				}
			}
			if(!deleted && (polygon == samplePlateShape_))
			{
				samplePlateShape_->deleteLater();
				samplePlateShape_ = 0;
				deleted = true;
			}
			if(!deleted && (polygon == cameraConfigurationShape_))
			{
				cameraConfigurationShape_->deleteLater();
				cameraConfigurationShape_ = 0;
				deleted = true;
			}
			if(!deleted)
			{
				emit sampleShapeDeleted(polygon);
			}
		}
	}
}

/// selects a rectangle which is under the cursor
void AMSampleCamera::selectCurrentShape(QPointF position)
{
	int i = 0;
	while(isValid(i) && !contains(position,i))
	{
		i++;
	}
	if(i <= index_)
	{

		setCurrentIndex(i);
		setShapeVectors(position);
	}
	else
	{
		setCurrentIndex(index_ + 1);
	}
}

/// moves the currently selected rectangle by position + currentVector_
void AMSampleCamera::moveCurrentShape(QPointF position, int index)
{
	position = undistortPoint(position);
	if(index == -1) index = currentIndex_;
	if(index <= index_ && isValid(index))
	{
		QVector3D newPosition;
		QVector3D oldPosition;
		QVector3D shift;
		if(moveOnShape() && samplePlateSelected_)
		{
			/// don't need to offset as just calculating the shift
			newPosition = getPointOnShape(samplePlateShape_,position);
			oldPosition = oldVector_;
			shift = newPosition - oldPosition;
			oldVector_ = newPosition;
		}
		else
		{
			QVector3D coordinate = shapeList_[index]->coordinate(TOPLEFT);
			double distance = depth(coordinate);
			newPosition = camera_->transform2Dto3D(position,distance);
			oldPosition = camera_->transform2Dto3D(currentVector_,distance);
			shift = newPosition - oldPosition;
		}
		shiftCoordinates(shift,index);
	}
	currentVector_ = position;
}

/// moves all rectangles by position + rectangleVector_[index]
void AMSampleCamera::moveAllShapes(QPointF position)
{
	/// \todo store oldPosition as a vector?
	position = undistortPoint(position);
	for(int i = 0; i <= index_; i++)
	{
		double distance = camera_->focalLength();
		QVector3D newPosition = camera_->transform2Dto3D(position,distance);
		QVector3D oldPosition = camera_->transform2Dto3D(currentVector_,distance);
		QVector3D shift = newPosition - oldPosition;
		shiftCoordinates(shift,i);
	}
	currentVector_ = (position);
}

/// assigns current vector to mouse position
void AMSampleCamera::setShapeVectors(QPointF position)
{
	position = undistortPoint(position);
	currentVector_ = position;
	oldVector_ = getPointOnShape(samplePlateShape_, position);

}

/// changes the depth of all shapes by the same amount
void AMSampleCamera::zoomAllShapes(QPointF position)
{
	for(int i = 0; i <= index_; i++)
	{

		QVector3D coordinate = shapeList_[i]->coordinate(TOPLEFT);
		QVector3D change = depthVector(coordinate)*(zoomPoint_.y()/position.y() - 1);
		shiftCoordinates(change,i);
	}
	zoomPoint_ = position;
}

/// set the zoom position
void AMSampleCamera::setZoomPoint(QPointF position)
{
	zoomPoint_ = position;
}

/// change the current rectangles rotation
void AMSampleCamera::rotateRectangle(QPointF position)
{
	if(isValid(currentIndex_))
	{
		double rotation = shapeList_[currentIndex_]->rotation()+(position - currentVector_).x();
		shapeList_[currentIndex_]->setRotation(rotation);
		updateShape(currentIndex_);
		currentVector_ = position;
	}
}

/// change the current rectangles depth
void AMSampleCamera::zoomShape(QPointF position)
{
	if(isValid(currentIndex_))
	{
		QVector3D coordinate = shapeList_[currentIndex_]->coordinate(TOPLEFT);
		QVector3D change = depthVector(coordinate)*(zoomPoint_.y()/position.y() - 1);
		shiftCoordinates(change,currentIndex_);
	}
	zoomPoint_ = position;

}

void AMSampleCamera::moveToSampleRequested(AMShapeData *shapeData){
	if(!moveToBeam())
		return;

	QVector3D currentPosition = getPointOnShape(shapeData, shapeData->shape()->at(0));
	QVector3D newPosition = beamIntersectionPoint(currentPosition, true);

	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::moveToSampleRequested. Current Position: %1 %2 %3 New Position: %4 %5 %6").arg(currentPosition.x()).arg(currentPosition.y()).arg(currentPosition.z()).arg(newPosition.x()).arg(newPosition.y()).arg(newPosition.z()) );

	QVector3D shift = newPosition - currentPosition;

	/// current
	double inboardOutboard;
	double upStreamDownStream;
	double upDown;
	if(!enableMotorMovement_)
	{
		for(int i = 0; i <= index_; i++)
		{
			shiftCoordinates(shift,i);
		}
		if(samplePlateSelected_)
			shiftCoordinates(shift,samplePlateShape_);
		motorCoordinate_ += shift;
		inboardOutboard = motorCoordinate().x();
		upStreamDownStream = motorCoordinate().y();
		upDown = motorCoordinate().z();

	}
	else
	{
		shift += motorCoordinate();
		inboardOutboard = shift.x();
		upStreamDownStream = shift.y();
		upDown = shift.z();

		moveMotors(inboardOutboard, upStreamDownStream, upDown);

	}
	emit motorCoordinateChanged(motorCoordinate());
	emit motorRotationChanged(motorRotation()->degrees());
}

AMAction3* AMSampleCamera::createMoveToSampleAction(const AMSample *sample){
	if(!moveToBeam() || !enableMotorMovement_)
		return 0;

	AMShapeData *shapeData = sample->sampleShapePositionData();
	QVector3D currentPosition = getPointOnShape(shapeData, shapeData->shape()->at(0));
	QVector3D newPosition = beamIntersectionPoint(currentPosition, true);

	QVector3D shift = newPosition - currentPosition;

	/// current
	double inboardOutboard;
	double upStreamDownStream;
	double upDown;

	shift += motorCoordinate();
	inboardOutboard = shift.x();
	upStreamDownStream = shift.y();
	upDown = shift.z();

	AMControlMoveAction3 *moveAction;
	AMControlInfo moveControlInfo;

	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3("Move to sample", "Move to sample"), AMListAction3::Parallel);

	moveControlInfo = ssaManipulatorX_->toInfo();
	moveControlInfo.setValue(inboardOutboard);
	moveAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(moveControlInfo), ssaManipulatorX_);
	retVal->addSubAction(moveAction);

	moveControlInfo = ssaManipulatorY_->toInfo();
	moveControlInfo.setValue(upStreamDownStream);
	moveAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(moveControlInfo), ssaManipulatorY_);
	retVal->addSubAction(moveAction);

	moveControlInfo = ssaManipulatorZ_->toInfo();
	moveControlInfo.setValue(upDown);
	moveAction = new AMControlMoveAction3(new AMControlMoveActionInfo3(moveControlInfo), ssaManipulatorZ_);
	retVal->addSubAction(moveAction);

	return retVal;
}

/// moves the clicked point to appear under the crosshair, and gives the predicted motor coordinate
void AMSampleCamera::shiftToPoint(QPointF position, QPointF crosshairPosition)
{
	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::shiftToPosition. Position coming in is %1, %2").arg(position.x()).arg(position.y()) );

	position = undistortPoint(position);
	AMShapeData* shape;

	if(isValid(currentIndex_))
	{
		shape = shapeList_[currentIndex_];
	}
	else if(samplePlateSelected_)
	{
		shape = samplePlateShape_;
	}
	else
		return;

	QVector3D oldCoordinate = shape->coordinate(TOPLEFT);
	QVector3D currentPosition = getPointOnShape(shape, position);/// don't use sample offset, will either click on sample or plate
	QVector3D newPosition = camera_->transform2Dto3D(crosshairPosition, depth(oldCoordinate));
	if(moveToBeam())
		newPosition = beamIntersectionPoint(currentPosition);
	QVector3D shift = newPosition - currentPosition;
	if(!debuggingSuppressed_)
	{
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::shiftToPoint. Current Position: %1 %2 %3").arg(currentPosition.x()).arg(currentPosition.y()).arg(currentPosition.z()) );
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::shiftToPoint. New Position: %1 %2 %3").arg(newPosition.x()).arg(newPosition.y()).arg(newPosition.z()) );
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::shiftToPoint. Manipulator Position: %1 %2 %3").arg(ssaManipulatorX_->value()).arg(ssaManipulatorY_->value()).arg(ssaManipulatorZ_->value()) );
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::shiftToPoint. Shift: %1, %2").arg(shift.x()).arg(shift.y()) );
	}
	/// current
	double inboardOutboard;
	double upStreamDownStream;
	double upDown;
	if(!enableMotorMovement_)
	{
		for(int i = 0; i <= index_; i++)
		{
			shiftCoordinates(shift,i);
		}
		if(samplePlateSelected_)
			shiftCoordinates(shift,samplePlateShape_);
		motorCoordinate_ += shift;
		inboardOutboard = motorCoordinate().x();
		upStreamDownStream = motorCoordinate().y();
		upDown = motorCoordinate().z();

	}
	else
	{
		shift += motorCoordinate();
		inboardOutboard = shift.x();
		upStreamDownStream = shift.y();
		upDown = shift.z();

		moveMotors(inboardOutboard, upStreamDownStream, upDown);

	}
	emit motorCoordinateChanged(motorCoordinate());
	emit motorRotationChanged(motorRotation()->degrees());


}

/// finishes the grouping rectangle
void AMSampleCamera::finishGroupRectangle(QPointF position)
{
	position = undistortPoint(position);
	QPointF topLeft = groupRectangle_.first();
	QPolygonF newPolygon = constructRectangle(topLeft,position);
	groupRectangle_ = newPolygon;
}

/// constructs the group rectangle
void AMSampleCamera::startGroupRectangle(QPointF position)
{
	position = undistortPoint(position);
	QPolygonF polygon = constructRectangle(position,position);
	groupRectangle_ = polygon;
}

/// adds points to the current polygon being drawn
void AMSampleCamera::drawShape(QPointF position)
{
	currentPolygon_<<position;
}

///  completes the current polygon being drawn
void AMSampleCamera::finishShape()
{
	if(currentPolygon_.count() < 3)
	{
		currentPolygon_.clear();
		return;
	}
	currentPolygon_<<currentPolygon_.first();

	/// add the shape to shapeList_
	/// undistort all the points
	/// get the depth
	/// get the coordinates
	double distance = camera_->focalLength();
	QVector<QVector3D> coordinates;
	for(int i = 0; i < currentPolygon_.count(); i++)
	{
		currentPolygon_[i] = undistortPoint(currentPolygon_.at(i));
		if(drawOnShapeValid())
		{
			coordinates<<getPointOnShape(currentPolygon_.at(i),getNormal(drawOnShape_), useSampleOffset());
		}
		else
		{
			coordinates<<camera_->transform2Dto3D(currentPolygon_.at(i),distance);
		}
	}

	AMShapeData* polygon = new AMShapeData();
	polygon->setCoordinateShape(coordinates);
	polygon->setName("Shape " + QString::number(index_+1));
	polygon->setRotation(0);
	polygon->setTilt(0);
	polygon->setYAxisRotation(0);
	insertItem(polygon);
	setCurrentIndex(index_);
	updateShape(index_);
	currentPolygon_.clear();

	emit shapeFinished();
	setOverrideMouseSelection(false);
}


void AMSampleCamera::startMultiDraw()
{
	currentPolygon_.clear();
}

/// updates the shape for the given index
void AMSampleCamera::updateShape(int index)
{
	/// must set shape based on coordinates
	if(isValid(index))
	{
		updateShape(shapeList_[index]);
	}

}

/// updates the visible shape of an AMShapeData
void AMSampleCamera::updateShape(AMShapeData *data)
{
	data->blockSignals(true);
	data->setShape(subShape(data));
	data->blockSignals(false);
	emit shapeDataChanged();
}

/// updates all shapes
void AMSampleCamera::updateAllShapes()
{
	for(int i = 0; i <= index_; i++)
	{
		updateShape(i);
	}
}

/// sets the coordinates for index
void AMSampleCamera::setCoordinates(QVector3D coordinate, int index)
{
	if(isValid(index))
	{
		// get the current center
		QVector3D center = centerCoordinate(index);
		QVector3D shift = coordinate - center;
		// shift the coordinates to the new center
		shiftCoordinates(shift,index);
	}
}

/// Overload of setCoordinates, takes three doubles and an index
void AMSampleCamera::setCoordinates(double x, double y, double z, int index)
{
	if(-1 == index) index = currentIndex_;
	QVector3D coordinate;
	coordinate.setX(x);
	coordinate.setY(y);
	coordinate.setZ(z);
	setCoordinates(coordinate, index);
}

/// places a grid
void AMSampleCamera::placeGrid(const QPointF &position)
{
	for(double i = 0; i < 20.0; i++)
	{
		for(double j = 0; j < 20.0; j++)
		{
			QPointF topLeft(i*0.05,j*0.05);
			QPointF bottomRight((i+1)*0.05,(j+1)*0.05);
			insertItem(new AMShapeData(constructRectangle(topLeft,bottomRight)));
			QVector<QVector3D> shape;
			for(int k = 0; k < 5; k++)
			{
				shape<<camera_->transform2Dto3D(shapeList_[index_]->shape()->at(k),camera_->focalLength());
			}
			shapeList_[index_]->setCoordinateShape(shape);
			shapeList_[index_]->setIdNumber(position.x());
			shapeList_[index_]->setTilt(0);
			shapeList_[index_]->setRotation(0);
			shapeList_[index_]->setYAxisRotation(0);
			updateShape(index_);
			setCurrentIndex(index_);
		}
	}

}

/// selects the first point for beam configuration
void AMSampleCamera::oneSelect()
{
	if(isValid(currentIndex_))
	{
		QVector<QVector3D> newBeamPosition = rotateShape(shapeList_[currentIndex_]);
		beamModel_->setPositionOne(newBeamPosition);
		emit beamChanged(beamModel_);
	}
	else if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_ONESELECT_INVALID_INDEX, QString("Invalid index of %1, cannot add beam.").arg(currentIndex_) );
}

/// selects the second point for beam configuration
void AMSampleCamera::twoSelect()
{
	if(isValid(currentIndex_))
	{
		QVector<QVector3D> newBeamPosition = rotateShape(shapeList_[currentIndex_]);
		beamModel_->setPositionTwo(newBeamPosition);

		emit beamChanged(beamModel_);
	}
	else if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_TWOSELECT_INVALID_INDEX, QString("Invalid index of %1, cannot add beam.").arg(currentIndex_) );
}

/// sets whether to actually move the motors
void AMSampleCamera::enableMotorMovement(bool isEnabled)
{
	enableMotorMovement_ = isEnabled;
	emit enableMotorMovementChanged(enableMotorMovement_);
}

/// starts or stops motor tracking
void AMSampleCamera::enableMotorTracking(bool isEnabled)
{
	if(isEnabled)
	{
		connect(ssaManipulatorX_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
		connect(ssaManipulatorY_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
		connect(ssaManipulatorZ_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
		connect(ssaManipulatorRot_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
	}
	else
	{
		disconnect(ssaManipulatorX_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
		disconnect(ssaManipulatorY_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
		disconnect(ssaManipulatorZ_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
		disconnect(ssaManipulatorRot_, SIGNAL(valueChanged(double)), this, SLOT(motorTracking(double)));
	}
	emit enableMotorTrackingChanged(isEnabled);
}

/// deletes all the shapes drawn in the camera calibration process
void AMSampleCamera::deleteCalibrationPoints()
{
	/// delete the calibration squares
	int index;
	AMShapeData *shapeToDelete;
	for(int i = 0; i < SAMPLEPOINTS; i++)
	{
		if(index_ >= 0)
		{
			if(calibrationPoints_[i] != 0)
			{
				index = shapeList_.indexOf(calibrationPoints_[i]);
				if(isValid(index))
				{
					shapeToDelete = takeItem(index);
					shapeToDelete->deleteLater();
				}
				// if the calibration point is not in the list, make
				// sure it is still deleted.
				else
				{
					calibrationPoints_[i]->deleteLater();
				}
				calibrationPoints_[i] = 0;
			}
		}
	}

}

/// sets the shape to draw on, and checks if it is valid
void AMSampleCamera::setDrawOnShape()
{
	if(isValid(currentIndex_))
	{
		drawOnShape_ = shapeList_[currentIndex_];
		drawOnShapeSelected_ = true;
	}
}

void AMSampleCamera::setDrawOnSamplePlate()
{
	if(samplePlateSelected_)
	{
		drawOnShape_ = samplePlateShape_;
		drawOnShapeSelected_ = true;
	}
}

/// Sets whether to draw on the plane of a shape.
void AMSampleCamera::setDrawOnShapeEnabled(bool enable)
{
	drawOnShapeEnabled_ = enable;

	emit drawOnShapeEnabledChanged(drawOnShapeEnabled_);
}

/// Adds a new beam marker at the specified position.  Deletes the old one.
void AMSampleCamera::setBeamMarker(QPointF position, int index)
{

	int removalIndex = shapeList_.indexOf(beamMarkers_[index]);
	AMShapeData* shapeToDelete;
	if(isValid(removalIndex))
	{
		shapeToDelete = takeItem(removalIndex);
		shapeToDelete->deleteLater();
	}

	startRectangle(position);
	beamMarkers_[index] = shapeList_[index_];
	beamMarkers_[index]->setName(QString("Beam Shape %1").arg(index + 1));

}

void AMSampleCamera::updateBeamMarker(QPointF position, int index)
{
	setCurrentIndex(shapeList_.indexOf(beamMarkers_[index]));
	finishRectangle(position);
}

/// Sets the beam shape
void AMSampleCamera::beamCalibrate()
{
	const int NUMBEROFRAYS = 4;
	const int NUMBEROFMARKERS = 3;

	for(int i = 0; i < NUMBEROFMARKERS; i++)
	{
		if(!beamMarkers_[i])
			return;
	}

	QList<QVector3D> ray [NUMBEROFRAYS];
	QVector<QVector3D> line [NUMBEROFRAYS];
	for(int i = 0; i < NUMBEROFRAYS; i++)
	{
		for(int j = 0; j < NUMBEROFMARKERS; j++)
		{
			ray[i]<<beamMarkers_[j]->coordinate(i);
		}
		line[i] = lineOfBestFit(ray[i]);
		beamModel_->setRay(line[i],i);

	}
	beamModel_->alignPositionTwo();
	saveBeam();


	if(shapeList_.isEmpty()) return;

	AMSamplePlate *plate = AMBeamline::bl()->samplePlate();

	for(int i = 0; i < 3; i++)
	{
		int index = shapeList_.indexOf(beamMarkers_[i]);
		if(index >= 0)
		{
			AMShapeData* polygon = takeItem(index);
			if(polygon == beamMarkers_[i])
			{
				if(plate != NULL)
				{
					plate->removeSample(plate->sampleFromShape(beamMarkers_[i]));
				}
				if(beamMarkers_[i])
				{
					beamMarkers_[i]->deleteLater();
					beamMarkers_[i] = 0;
					polygon = 0;
				}
			}
			else
			{
				if(!debuggingSuppressed_)
					AMErrorMon::debug(this, AMSAMPLECAMERA_BEAMCALIBRATE_ERROR_DELETING_SHAPES, QString("Error deleting shapes with index %1 in beamCalibrate.").arg(index) );
				insertItem(polygon);
			}
		}
	}

}

void AMSampleCamera::createBeamShape(int index)
{
	int removalIndex = shapeList_.indexOf(beamMarkers_[index]);
	AMShapeData* shapeToDelete;
	if(isValid(removalIndex))
	{
		shapeToDelete = takeItem(removalIndex);
		shapeToDelete->deleteLater();
	}
	startRectangle(QPointF(0.5,0.5));
	beamMarkers_[index] = shapeList_[index_];
	beamMarkers_[index]->setName(QString("Beam Shape %1").arg(index + 1));
	QVector3D topRight = getWidthNormal(beamMarkers_[index]);
	QVector3D base = beamMarkers_[index]->coordinate(TOPLEFT);
	QVector3D bottomLeft = -0.1*getHeightNormal(beamMarkers_[index]);
	beamMarkers_[index]->setCoordinate((base+topRight),TOPRIGHT);
	beamMarkers_[index]->setCoordinate((base+bottomLeft),BOTTOMLEFT);
	beamMarkers_[index]->setCoordinate(base+bottomLeft+topRight, BOTTOMRIGHT);
	for(int i = 0; i < 3; i++)
	{
		if(beamMarkers_[i] && i != index)
			beamMarkers_[i]->setVisible(false);
	}
}

void AMSampleCamera::moveBeamShape(QPointF point, int index)
{
	int shapeIndex = shapeList().indexOf(beamMarkers_[index]);
	moveCurrentShape(point,shapeIndex);
}

void AMSampleCamera::beamMousePressed(QPointF point, int index)
{
	Q_UNUSED(index)
	setShapeVectors(point);
}

/// Sets the sample plate (shape) to be the currently selected sample.  Removes the
/// sample from the sample plate (class) and uses the sample's shape for the shape of the sample plate.
void AMSampleCamera::setSamplePlate()
{
	if(isValid(currentIndex_))
	{
		for(int i = 0; i < 3; i++)
		{
			if(shapeList_[currentIndex_] == beamMarkers_[i])
			{
				if(!debuggingSuppressed_)
					AMErrorMon::debug(this, AMSAMPLECAMERA_SETSAMPLEPLATE_BAD_INDEX, QString("Cannot set a sample plate to be a beam shape. Shape list index %1").arg(currentIndex_) );
				samplePlateSelected_ = false;
				return;
			}
		}
		// Make sure to delete the old shape if there is one.
		if(samplePlateShape_)
		{
			delete(samplePlateShape_);
		}
		// remove from the list of samples.
		samplePlateShape_ = new AMShapeData();
		samplePlateShape_->copy(currentShape());


		AMSamplePlate *currentPlate = AMBeamline::bl()->samplePlate();
		AMSample *currentSample = currentPlate->sampleFromShape(currentShape(currentIndex_));
		AMShapeData* currentShape = takeItem(currentIndex_);
		currentShape->deleteLater();
		if(currentSample)
		{
			currentPlate->removeSample(currentSample);
		}

		samplePlateShape_->setName("Sample Plate");
		samplePlateShape_->setOtherDataFieldOne("Sample Plate");
		samplePlateSelected_ = true;
		setDrawOnSamplePlate();

		setDrawOnShapeEnabled(true);



	}

}

void AMSampleCamera::setSamplePlate(AMShapeData *samplePlate)
{
	samplePlateShape_ = samplePlate;
	if(samplePlateShape_->name() != "Sample Plate")
	{
		samplePlateShape_->setName("Sample Plate");
	}
	samplePlateShape_->setOtherDataFieldOne("Sample Plate");
	samplePlateSelected_ = true;
	setDrawOnSamplePlate();
}

void AMSampleCamera::setSimpleSamplePlate(QVector3D base, QVector3D width, QVector3D height)
{
	/// Construct the shape

	QVector<QVector3D> simpleSamplePlateShape;
	simpleSamplePlateShape <<base
			      <<base + width
			     <<base + width + height
			    <<base + height
			   <<base;
	/// set the sample shape data
	AMShapeData *simpleSamplePlate = new AMShapeData();
	simpleSamplePlate->setCoordinateShape(simpleSamplePlateShape);
	/// have to zero the old rotation so that the new shape may be properly rotated.
	oldRotation_->setDegrees(0);
	simpleSamplePlate->setCoordinateShape(applyMotorRotation(simpleSamplePlate,motorRotation()->radians()));
	updateShape(simpleSamplePlate);
	/// set the shape data as a sample plate
	setSamplePlate(simpleSamplePlate);
	/// fix oldRotation_
	oldRotation_->setDegrees(motorRotation()->degrees());
}

/// Create the sample plate using the 3D and 2D information, plus rotations, provided by the
/// SamplePlate Wizard.
void AMSampleCamera::createSamplePlate(QVector<QVector3D> coordinates, QVector<QPointF> points, QVector<double> rotations, int numberOfPoints)
{
	const int NUMBER_OF_COORDINATES = numberOfPoints;
	const int MINIMUM_NUMBER_OF_POINTS = NUMBER_OF_COORDINATES*3; // minimal shape is a triangle, 3 points for each coordinate.

	/// check that the expected number of coordinates matches the number of coordinates given
	if(coordinates.count() != NUMBER_OF_COORDINATES && !debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_CREATESAMPLEPLATE_UNEXPECTED_NUMBER_OF_COORDINATES, QString("There were an unexpected number of coordinates passed to createSamplePlate. Expected: %1 Received: %2").arg(NUMBER_OF_COORDINATES).arg(coordinates.count()) );
	/// check that the minimum number of points have been provided. If less than the
	/// minimum number are provided, either some points were missed, or the
	/// sample plate boundary was not properly defined (ex only two points per position)
	if(points.count() < MINIMUM_NUMBER_OF_POINTS && !debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_CREATESAMPLEPLATE_TOO_FEW_POINTS, QString("Too few points passed to createSamplePlate. Required: %1 Received: %2").arg(points.count()).arg(MINIMUM_NUMBER_OF_POINTS) );
	/// for n number of coordinates we need to do n(n-1)/2 iterations
	/// to get n-1 results to average
	/// need to do this for each vertex of the plate
	// points should be in n-pairs for each vertex in that order
	int numberOfVertices = points.count()/NUMBER_OF_COORDINATES;
	QPointF *pointList = new QPointF [numberOfVertices*NUMBER_OF_COORDINATES];
	/// fill pointList with the values from "points".
	for(int i = 0; i < numberOfVertices; i++)
	{
		for(int j = 0; j < NUMBER_OF_COORDINATES; j++)
		{
			pointList[i*NUMBER_OF_COORDINATES + j] = points.at(i*NUMBER_OF_COORDINATES + j);
		}
	}


	QVector< QVector<QVector3D> > pointResults;
	QVector<QPointF> vertexPoints;
	for(int i = 0; i < numberOfVertices; i++)
	{
		for(int j = 0; j < NUMBER_OF_COORDINATES; j++)
		{
			vertexPoints<<pointList[i*NUMBER_OF_COORDINATES + j];
		}
		pointResults<<findSamplePlateCoordinate(coordinates,vertexPoints,rotations,numberOfPoints);
		vertexPoints.clear();
	}


	QVector<QVector3D> plateShape;

	for(int i = 0; i < numberOfVertices; i++)
	{
		plateShape<<pointResults.at(i).last();
	}

	AMShapeData *samplePlateShape = new AMShapeData();
	samplePlateShape->setCoordinateShape(plateShape);
	samplePlateShape->setName("Sample Plate");
	samplePlateShape->setOtherDataFieldOne("Sample Plate");
	setSamplePlate(samplePlateShape);

	samplePlateShape_->setVisible(true);
}




/// saves the sample plate to the database by creating a sample
/// and storing it as a sample
void AMSampleCamera::saveSamplePlate()
{
	if(samplePlateSelected_ && samplePlateShape_)
	{
		AMSample* samplePlate = new AMSample();
		samplePlate->setName(samplePlateShape_->name());
		samplePlate->setSampleShapePositionData(samplePlateShape_);
		AMControlSet *samplePositioner = AMBeamline::bl()->currentSamplePositioner();
		if(samplePositioner){
			QString positionAsNote;
			for(int x = 0; x < samplePositioner->count(); x++)
				positionAsNote.append(QString("%1;").arg(samplePositioner->at(x)->value()));

			samplePlate->setNotes(positionAsNote);
		}
		samplePlate->storeToDb(AMDatabase::database("user"));
	}
}

void AMSampleCamera::setCameraConfigurationShape()
{
	if(isValid(currentIndex_))
	{
		cameraConfigurationShape_ = shapeList_[currentIndex_];
		cameraConfigurationShape_->setName("Configuration shape");
		cameraConfigurationShape_->setOtherDataFieldOne("Config");
	}
}

/// Solve for the centre of rotation
void AMSampleCamera::configureRotation(const QVector<QVector3D> coordinates, const QVector<QPointF> points, const QVector<AMAngle *> rotations, const int numberOfPoints)
{
	typedef QPair<QVector3D,QVector3D> VectorPair;
	/// solve the following equations:
	/// CoRn + ln - tnVn = Bn
	/// CoR = centre of rotation
	/// ln = vector from CoR to Cn
	/// Cn is the C0 after rotation
	/// tnvn is the vector from Bn to Cn
	/// Bn is a point that lies along the line corresponding to points[n]

	/// l0 = vector from CoR to C0
	/// ln = vector from CoR to Cn
	/// MC = motor Coordinate
	/// C0 == MC
	/// Cn = rotated C0


	/// since a particular point (C0) is always known, it should not be used in configuration
	/// instead, use a series of points in reference to known points
	/// i.e rotatedOffset-offset=rotatedPoint -original point
	/// ... rotatedOffset-offset-tnvn=Bn-MCn
	/// need only two points to solve for this

	/// the equation is ln-l0-tnvn=Bn-MCn
	/// the matrix solves for l0, tn
	/// -l0 is the offset from the motor coordinate to centre of rotation:
	/// since the motor coordinate is the non-rotated point
	/// Since C0 must correspond to MC (so that C0 is known)
	/// l0 is -1*rotationalOffset

	/// Matrix calculation looks like this:

	/// [ r1-I v1 0  0  ] [ l0 ]   [ B1-MC1 ]
	/// [ r2-I 0  v2 0  ] [ t1 ] = [ B2-MC2 ]
	/// [ r3-I 0  0  v3 ] [ t2 ]   [ B3-MC3 ]
	///                   [ t3 ]
	/// solves for l0,t1,t2,t3...
	/// then -l0 is the rotational offset, since the motor coordinate is C0
	/// rn is ~ ln/l0



	if(numberOfPoints != coordinates.count() || numberOfPoints != points.count() || numberOfPoints != rotations.count())
	{
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_CONFIGUREROTATION_LENGTH_MISMATCH, QString("Number of points doesn't match list length for configureRotation. Number of points: %1 Coordinate count: %2 Points count: %3 Rotations count: %4").arg(numberOfPoints).arg(coordinates.count()).arg(points.count()).arg(rotations.count()) );
		return;
	}

	// need to find v1, B1, v2, B2 ...
	// Bn is some point along the line clicked on, vn is a unit vector in the direction of that line
	QVector<VectorPair> lines;
	for(int i = 0; i < numberOfPoints; i++)
	{
		lines<<findScreenVector((points.at(i)),coordinates.at(i),coordinates.at(i) + QVector3D(i+1,i+1,i+1));
	}
	QVector<QVector3D> bases,vectors;

	foreach( VectorPair line, lines)
	{
		bases<<line.first;
		vectors<<line.second;
	}


	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::configureRotation - about to construct matrices") );

	/// Construct the two known matrices
	MatrixXd matrix = constructCentreOfRotationMatrix(rotations,vectors, numberOfPoints);
	MatrixXd coordinateMatrix = constructCentreOfRotationCoordinateMatrix(coordinates, bases, numberOfPoints);

	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::configureRotation - done constructing matrices") );

	/// solve for the unknown part
	MatrixXd solution = solveCentreOfRotationMatrix(matrix,coordinateMatrix);
	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::configureRotation - done solving") );

	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Input matrix") );
	debugPrintMatrix(matrix);
	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("RHS matrix") );
	debugPrintMatrix(coordinateMatrix);
	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Solution Matrix") );
	debugPrintMatrix(solution);
	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Computed RHS") );
	debugPrintMatrix(matrix*solution);

	/// This requires that your are clicking on the point coordinates.at(0)
	QVector3D centreOfRotationOffset = matrixToVector(solution);
	QVector3D centreOfRotation = coordinates.at(0) - centreOfRotationOffset;

	setRotationalOffset(centreOfRotation - coordinates.at(0));
	updateView();

}

void AMSampleCamera::saveRotationalOffset()
{
	rotationalOffset_->setName(QDateTime::currentDateTime().toString("dd MM yyyy - hh:mm:ss ap"));
	rotationalOffset_->storeToDb(AMDatabase::database("SGMPublic"));
}

void AMSampleCamera::moveSamplePlateTo(const QVector3D &coordinate)
{
	if(samplePlateSelected_)
	{

		samplePlateShape_->shiftTo(coordinate);
		updateShape(samplePlateShape_);

	}
}

void AMSampleCamera::moveSamplePlate(int movement)
{
	if(samplePlateSelected_)
	{
		QVector3D direction = getWidthNormal(samplePlateShape_);
		double shapeWidth = (samplePlateShape_->coordinate(1) - samplePlateShape_->coordinate(0)).length();
		QVector3D shiftMovement = direction * movement/1000 * shapeWidth;
		samplePlateShape_->shift(shiftMovement);
		updateShape(samplePlateShape_);
	}
}

void AMSampleCamera::moveSamplePlate(QPointF position)
{
	QVector3D newPosition;
	QVector3D oldPosition;
	QVector3D plateShift;
	position = undistortPoint(position);
	newPosition = getPointOnShape(samplePlateShape_,position);
	oldPosition = oldVector_;
	plateShift = newPosition - oldPosition;
	samplePlateShape_->shift(plateShift);
	oldVector_ = newPosition;
}

void AMSampleCamera::addBeamMarker(int index)
{
	if(beamMarkers_[index])
	{
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_ADDBEAMMARKER_NULL_MARKER, QString("Beam marker is null at index %1.").arg(index) );
		deleteShape(shapeList_.indexOf(beamMarkers_[index]));
	}
	AMShapeData* newBeamShape = new AMShapeData();

	if(samplePlateSelected_)
	{
		QVector<QVector3D> coordinateShape = findIntersectionShape(samplePlateShape_);
		if(!coordinateShape.isEmpty())
		{
			newBeamShape->setCoordinateShape(coordinateShape);
			newBeamShape->setName(QString("Beam Marker %1").arg(index));
			newBeamShape->setRotation(0);
			newBeamShape->setTilt(0);
			newBeamShape->setYAxisRotation(0);
			insertItem(newBeamShape);
			beamMarkers_[index] = newBeamShape;
			setCurrentIndex(index_);
			updateShape(currentIndex_);
		}
	}

}

void AMSampleCamera::updateView()
{
	emit dataChanged(index(0), index(shapeList_.count()));
}

void AMSampleCamera::moveMotorTo(QVector3D coordinate, double rotation)
{
	double x = coordinate.x();
	double y = coordinate.y();
	double z = coordinate.z();
	if(enableMotorMovement_)
	{
		bool success = moveMotors(x,y,z,rotation);
		if(!success && !debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_MOVEMOTORTO_FAILED_TO_MOVE, QString("Failed to move motors in moveMotorTo (%1, %2, %3, %4)").arg(x).arg(y).arg(z).arg(rotation) );
	}
	else if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_MOVEMOTORTO_CANNOT_MOVE_MOTORS, QString("Cannot move motors because enableMotorMovement is off.") );
}

void AMSampleCamera::stopMotors()
{
	if(enableMotorMovement_)
	{
		ssaManipulatorRot_->stop();
		ssaManipulatorX_->stop();
		ssaManipulatorY_->stop();
		ssaManipulatorZ_->stop();
	}
	else if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_STOPMOTORS_CANNOT_STOP_MOTORS, QString("Cannot stop motors because enableMotorMovement is off") );
}

void AMSampleCamera::addSample(AMSample *sample){
	int shapeIndex = indexOfShape(sample->sampleShapePositionData());
	if(shapeIndex == -1){
		sample->sampleShapePositionData()->setName(sample->name());
		insertItem(sample->sampleShapePositionData());
	}
}


/// Removes the sample from the list but does not delete it.  It is still referenced by
/// the AMSample. Used for cleaning up the camera on sample plate change.
void AMSampleCamera::removeSample(AMSample *sample){
	int shapeIndex = indexOfShape(sample->sampleShapePositionData());
	AMShapeData* shapeRetrieved;
	if(shapeIndex >= 0)
	{
		bool blockState = blockSignals(true);
		shapeRetrieved = takeItem(shapeIndex);
		blockSignals(blockState);
		if(shapeRetrieved != sample->sampleShapePositionData() && !debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_REMOVESAMPLE_FAILED, QString("Failed to remove sample at index %1.").arg(shapeIndex) );
	}
}

void AMSampleCamera::removeShapeData(AMShapeData *shape)
{
	AMShapeData *shapeToDelete;
	if(!shape)
	{
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_REMOVESHAPEDATA_FAILED_NULL_SHAPE, QString("Call to removeShapeData failed because a null shape was passed as an argument.") );
		return;
	}
	if(shapeList_.contains(shape))
	{
		int takeIndex = shapeList_.indexOf(shape);
		shapeToDelete = takeItem(takeIndex);
		if(shape != shapeToDelete)
		{
			if(!debuggingSuppressed_)
				AMErrorMon::debug(this, AMSAMPLECAMERA_REMOVESHAPEDATA_FAILED, QString("Failed to remove shape data at index %1.").arg(takeIndex) );
			insertItem(shapeToDelete);
		}
		else
		{
			shape->deleteLater();
			shape = 0;
		}
	}
}

void AMSampleCamera::loadDefaultBeam()
{
	QVector<QVector3D> beamOne;
	QVector<QVector3D> beamTwo;
	beamOne<<QVector3D(9.6,2,-9.96)<<QVector3D(10.6,2,-9.96)<<QVector3D(10.6,2,-10.06)<<QVector3D(9.6,2,-10.06)<<QVector3D(9.6,2,-9.96);
	beamTwo<<QVector3D(9.6,0,-9.96)<<QVector3D(10.6,0,-9.96)<<QVector3D(10.6,0,-10.06)<<QVector3D(9.6,0,-10.06)<<QVector3D(9.6,0,-9.96);
	beamModel_->setPositionOne(beamOne);
	beamModel_->setPositionTwo(beamTwo);
	saveBeam();
}

void AMSampleCamera::loadDefaultCamera()
{
	AMCameraConfiguration* defaultConfiguration = new AMCameraConfiguration();
	QVector<QVector3D> cameraMatrix;
	cameraMatrix<<QVector3D(0.0194118,-0.000294812,-0.00117781)<<QVector3D(0.00245398,0.00826314,0.000605619)
		   <<QVector3D(0.000514431,-0.0221707,3.75237e-05)<<QVector3D(-0.185726,-0.243429,0.951474);
	defaultConfiguration->setCameraMatrix(cameraMatrix);
	defaultConfiguration->setName("Default Camera Configuration");
	defaultConfiguration->setCameraDistortion(-0.09);
	defaultConfiguration->setCameraFocalLength(0.41);
	defaultConfiguration->setCameraFOV(0.387);
	defaultConfiguration->setPixelAspectRatio(1);
	camera_->setCameraConfiguration(defaultConfiguration);
	saveCamera();

}

/// Loads the default Sample plate shape.
/// This is a sample plate of 20mmx20mm, set back 2mm.
void AMSampleCamera::loadDefaultSamplePlate()
{
	AMShapeData* samplePlate = new AMShapeData();
	QVector<QVector3D> samplePlateShape;
	samplePlateShape<<QVector3D(0,2,0)<<QVector3D(20,2,0)<<QVector3D(20,2,-20)<<QVector3D(0,2,-20)<<QVector3D(0,2,0);
	samplePlate->setCoordinateShape(samplePlateShape);
	setSamplePlate(samplePlate);
	saveSamplePlate();
}

void AMSampleCamera::setSamplePlateOffsetPlate()
{
	/// maybe make this an option
	sampleOffset_ = 0.75;
	/// \todo this is a rough guess
	/// 1mm precision is not good enough to measure this
}

void AMSampleCamera::setSamplePlateOffsetWafer()
{
	sampleOffset_ = 0.80;
	/// once again, rough guess
	/// need a more accurate measurement
}

void AMSampleCamera::setSamplePlateOffset(double offset)
{
	sampleOffset_ = offset;
}

void AMSampleCamera::setCameraConfigured(bool cameraConfigured){
	cameraConfigured_ = cameraConfigured;
}

void AMSampleCamera::setRotationConfigured(bool rotationConfigured){
	rotationConfigured_ = rotationConfigured;
}

void AMSampleCamera::setSamplePlateConfigured(bool samplePlateConfigured){
	samplePlateConfigured_ = samplePlateConfigured;
}

void AMSampleCamera::setBeamConfigured(bool beamConfigured){
	beamConfigured_ = beamConfigured;
}

bool AMSampleCamera::debuggingSuppressed()
{
	return debuggingSuppressed_;
}

void AMSampleCamera::setDebuggingSuppressed(bool value)
{
	debuggingSuppressed_ = value;
}

/// tracks motor movement and shifts drawings accordingly
void AMSampleCamera::motorTracking(double)
{
	/// update coordinates, line edits, don't move the motor
	double tolerance = 0.01;
	double motorX = ssaManipulatorX_->value();
	double motorZ = ssaManipulatorZ_->value();
	double motorY = ssaManipulatorY_->value();
	double currentMotorRotation = ssaManipulatorRot_->value();
	if(fabs(motorX-motorCoordinate_.x()) < tolerance && fabs(motorY-motorCoordinate_.y()) < tolerance && fabs(motorZ-motorCoordinate_.z()) < tolerance && fabs(currentMotorRotation-motorRotation()->degrees()) < tolerance)
	{
		return;
	}

	emit motorMoved();

	motorUpdateDeferredFunction_.schedule();
}

void AMSampleCamera::motorsFinishedMoving()
{
	if(!ssaManipulatorX_->isMoving())
		if(!ssaManipulatorY_->isMoving())
			if(!ssaManipulatorZ_->isMoving())
				if(!ssaManipulatorRot_->isMoving())
					emit moveSucceeded();
}

void AMSampleCamera::onShapeDataChanged()
{

}

void AMSampleCamera::saveBeam()
{
	AMDatabase* db = AMDatabase::database("SGMPublic");
	QList<int> matchList = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(),"name", beamModel_->name());
	if(matchList.count() != 0)
	{
		beamModel_->dissociateFromDb();
		QVariantList beamList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(), "name");
		beamModel_->setName(QDateTime::currentDateTime().toString("dd MM yyyy - hh:mm:ss ap"));
	}
	beamModel_->storeToDb(db);
}

void AMSampleCamera::saveCamera()
{
	AMCameraConfiguration* cameraConfiguration = camera_->cameraConfiguration();
	AMDatabase* db = AMDatabase::database("SGMPublic");
	cameraConfiguration->setName(QDateTime::currentDateTime().toString("dd MM yyyy - hh:mm:ss ap"));
	cameraConfiguration->storeToDb(db);
}

/// Constructor
AMSampleCamera::AMSampleCamera(QObject *parent) :
	QAbstractListModel(parent)
{
	debuggingSuppressed_ = true;
	crosshair_ = QPointF(0.5,0.5);
	crosshairLocked_ = false;
	index_ = -1;
	camera_ = new AMCamera();
	beamModel_ = new AMBeamConfiguration();
	centerOfRotation_ = QVector3D(0,0,0);
	directionOfRotation_ = QVector3D(0,-1,0);

	if(QApplication::instance()->arguments().contains("--disableMotorMovement"))
		enableMotorMovement_ = false;
	else
		enableMotorMovement_ = true;

	drawOnShapeEnabled_ = false;
	drawOnShapeSelected_ = false;
	samplePlateSelected_ = false;
	sampleOffset_ = 0;

	distortion_ = true;

	overrideMouseSelection_ = false;
	moveToBeam_ = true;
	moveOnShape_ = true;

	rotationalOffset_ = new AMRotationalOffset(10.5,2.35,0);

	cameraConfigured_ = false;
	rotationConfigured_ = false;
	samplePlateConfigured_ = false;
	beamConfigured_ = false;

	emit rotationalOffsetChanged(rotationalOffset());

	oldRotation_ = new AMAngle();

	oldRotation_->setRadians(0);
	samplePlateShape_ = 0; //NULL


	for(int i= 0; i < SAMPLEPOINTS; i++)
	{
		calibrationPoints_[i] = new AMShapeData();
		if(i < 3)
		{
			beamMarkers_[i] = new AMShapeData();
		}
	}


	AMDatabase *db = AMDatabase::database("SGMPublic");

	if(!db){

		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_SEVERE_STARTUP_DATABASE_ISSUES, QString("This is very bad. AMSampleCamera is trying to make its own databases.") );
		db = AMDatabase::database("user");
		// create the database
		db = AMDatabase::createDatabase("user", "/home/sgm/AcquamanData/userdata.db");
		if(!db)
		{
			if(!debuggingSuppressed_)
				AMErrorMon::debug(this, AMSAMPLECAMERA_SEVERE_STARTUP_DATABASE_ISSUES, QString("This is very bad. AMSampleCamera tried to make its own databases and failed.") );
		}
		else
		{
			bool success = true;

			AM::registerTypes();
			success &= AMDbObjectSupport::s()->registerDatabase(db);
			success &= AMDbObjectSupport::s()->registerClass<AMDbObject>();
			success &= AMDbObjectSupport::s()->registerClass<AMSamplePre2013>();
			success &= AMDbObjectSupport::s()->registerClass<AMCameraConfiguration>();
			success &= AMDbObjectSupport::s()->registerClass<AMBeamConfiguration>();
			success &= AMDbObjectSupport::s()->registerClass<AMSample>();
			success &= AMDbObjectSupport::s()->registerClass<AMSamplePlatePre2013>();
			success &= AMDbObjectSupport::s()->registerClass<AMRotationalOffset>();
			if(!debuggingSuppressed_)
				AMErrorMon::debug(this, AMSAMPLECAMERA_SEVERE_STARTUP_DATABASE_ISSUES, QString("This is very bad. AMSampleCamera made its own databases and registration was successful? %1.").arg(success) );
		}
	}



	QList<int> matchIDs = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(),"name","defaultConfiguration");
	if(matchIDs.count() == 0)
	{
		QVector<QVector3D> positionOne;
		positionOne<<QVector3D(0.02,0.02,0)<<QVector3D(-0.02,0.02,0)<<QVector3D(-0.02,-0.02,0)<<QVector3D(0.02,-0.02,0)<<QVector3D(0.02,0.02,0);
		QVector<QVector3D> positionTwo;
		positionTwo<<QVector3D(0.02,0.02,1)<<QVector3D(-0.02,0.02,1)<<QVector3D(-0.02,-0.02,1)<<QVector3D(0.02,-0.02,1)<<QVector3D(0.02,0.02,1);
		beamModel_->setPositionOne(positionOne);
		beamModel_->setPositionTwo(positionTwo);
		beamModel_->setName("defaultConfiguration");
		bool success = beamModel_->storeToDb(db);
		if(!success && !debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_FAILED_TO_STORE_DEFAULT_BEAM, QString("Failed to store default beam to the database.") );
	}
	else
		beamModel_->loadFromDb(db,matchIDs.first());

	/// add motor manipulators

	ssaManipulatorX_ = 0; //NULL
	ssaManipulatorY_ = 0; //NULL
	ssaManipulatorZ_ = 0; //NULL
	ssaManipulatorRot_ = 0; //NULL

	motorRotation_ = new AMAngle();
	connect(&motorUpdateDeferredFunction_, SIGNAL(executed()), this, SLOT(setMotorCoordinate()));
	connect(camera_, SIGNAL(configurationChanged(AMCameraConfiguration*)),this, SIGNAL(cameraConfigurationChanged(AMCameraConfiguration*)));
}

double AMSampleCamera::manipulatorX()
{
	return ssaManipulatorX_->value();
}

double AMSampleCamera::manipulatorY()
{
	return ssaManipulatorY_->value();
}

double AMSampleCamera::manipulatorZ()
{
	return ssaManipulatorZ_->value();
}

double AMSampleCamera::manipulatorR()
{
	return ssaManipulatorRot_->value();
}


/// shifts all coordinates by  the specified shift
void AMSampleCamera::shiftCoordinates(QVector3D shift, int index)
{
	if(isValid(index))
	{
		shiftCoordinates(shift,shapeList_[index]);
	}
}

void AMSampleCamera::shiftCoordinates(QVector3D shift, AMShapeData *shape)
{
	if(shape)
	{
		shape->shift(shift);
		updateShape(shape);
	}
}



void AMSampleCamera::applyMotorRotation(int index, double rotation)
{
	if(isValid(index))
	{
		shapeList_[index]->setCoordinateShape(applyMotorRotation(shapeList_[index],rotation));
	}
}

QVector<QVector3D> AMSampleCamera::applyMotorRotation(AMShapeData *shape, double rotation) const
{
	return applySpecifiedRotation(shape, directionOfRotation_, centerOfRotation_, rotation - oldRotation_->radians())->coordinates();
}


AMShapeData* AMSampleCamera::applySpecifiedRotation(const AMShapeData *shape, QVector3D direction, QVector3D centre, double angle) const
{
	AMShapeData* newShape = new AMShapeData();
	newShape->copy(shape);
	for(int i = 0; i < newShape->count(); i++)
	{
		newShape->setCoordinate(rotateCoordinate(newShape->coordinate(i),centre,direction,AMAngle(angle,AMAngle::RAD)),i);
	}
	return newShape;
}

AMShapeData* AMSampleCamera::applySpecifiedRotation(const AMShapeData *shape, QVector3D direction, double angle) const
{
	QVector3D centre = shape->centerCoordinate();
	return applySpecifiedRotation(shape,centre,direction,angle);
}

AMShapeData* AMSampleCamera::applySpecifiedRotation(const AMShapeData* shape, AMSampleCamera::AxisDirection direction) const
{
	double angle;
	QVector3D axis;
	if(direction == XAXIS)
	{
		angle = shape->tilt();
		axis = QVector3D(1,0,0);
	}
	else if(direction == YAXIS)
	{

		angle = shape->yAxisRotation();
		axis = QVector3D(0,1,0);
	}
	else if(direction == ZAXIS)
	{
		angle = shape->rotation();
		axis = QVector3D(0,0,1);
	}
	else
	{
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_ROTATION_AXIS_NOT_SPECIFIED, QString("There was no axis specified for the rotation, using Z axis by default.") );
		angle = shape->rotation();
		axis = QVector3D(0,0,1);
	}
	return applySpecifiedRotation(shape,axis,angle);
}

/// rotates a coordinate about the given line, by the given amount
/// \param coordinate: the coordinate to rotate
/// \param center: the point about which to rotate the coordinate
/// \param direction: the direction of the axis of rotation
/// \param rotation: the amount to rotate by, in radians
QVector3D AMSampleCamera::rotateCoordinate(QVector3D coordinate, QVector3D center, QVector3D direction, AMAngle rotation) const
{
	/* this might be where the rotation problem lies */
	/// try using a matrix approach instead, it is much cleaner and easier to follow
	return rotateCoordinateByMatrix(coordinate, center, direction, rotation);
	/// not sure if the approach below is correct, it is based off the the general
	/// rotation matrix, but since it is done without any matrices, it is hard
	/// to find errors.
	double u = direction.x();
	double v = direction.y();
	double w = direction.z();

	double x = coordinate.x();
	double y = coordinate.y();
	double z = coordinate.z();


	/// Each new coordinate has three terms - a 1-cos, a cos and a sin term
	/// to simplify the math, they will be computed as 3D vectors
	/// the xyz components will be the dot product of the terms with the trig terms

	QVector3D trigTerms(1-cos(rotation.radians()),cos(rotation.radians()),sin(rotation.radians()));

	/// as well, all terms contain a -coordinate.direction component

	double directionTerm = dot(coordinate,direction);

	/// all terms also use centerXdirection and directionXcoordinate
	QVector3D cXd = QVector3D::crossProduct(center,direction);
	QVector3D dXc = QVector3D::crossProduct(direction,coordinate);


	QVector3D terms [3];
	terms[0] = QVector3D((pow(v,2)+pow(w,2)),(-1*u*v),(-1*u*w));
	terms[1] = QVector3D((-1*v*u),(pow(u,2)+pow(w,2)),(-1*v*w));
	terms[2] = QVector3D((-1*u*w),(-1*v*w),(pow(u,2)+pow(v,2)));

	double coord [3];
	coord[0] = x;
	coord[1] = y;
	coord[2] = z;

	double centerXDistance[3];
	centerXDistance[0] = cXd.x();
	centerXDistance[1] = cXd.y();
	centerXDistance[2] = cXd.z();

	double distanceXCoordinate [3];
	distanceXCoordinate[0] = dXc.x();
	distanceXCoordinate[1] = dXc.y();
	distanceXCoordinate[2] = dXc.z();

	double directions [3];
	directions[0] = direction.x();
	directions[1] = direction.y();
	directions[2] = direction.z();
	///complete the direction must do u*directionTerm


	QVector3D vector [3];
	double component [3];

	for(int i = 0; i < 3; i++)
	{
		vector[i] = QVector3D(dot(center,terms[i]) + directions[i]*directionTerm,coord[i],centerXDistance[i]+distanceXCoordinate[i]);
		component[i] = dot(vector[i],trigTerms);
	}

	QVector3D rotatedCoordinate(component[0],component[1],component[2]);
	return rotatedCoordinate;


}

QVector3D AMSampleCamera::rotateCoordinateByMatrix(QVector3D coordinate, QVector3D center, QVector3D direction, AMAngle rotation) const
{
	MatrixXd rotationMatrix = constructRotationMatrix(direction,rotation.radians());
	/// subtract the centre of rotation to the coordinate to shift the frame
	// the rotation matrix is centred at 0, so must shift
	MatrixXd originalPosition = vectorToMatrix(coordinate - center);
	MatrixXd newPosition = rotationMatrix * originalPosition;
	/// add center back to shift back to 0-centered coordinates
	QVector3D newCoordinate = matrixToVector(newPosition) + center;
	return newCoordinate;
}

/// applies rotation, tilt, and distortion to the shape
QPolygonF AMSampleCamera::subShape(const AMShapeData* shape) const
{
	if(!(shape) && !debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_NULL_SUBSHAPE, QString("A call to subShape was made with a null shape.") );
	AMShapeData* rotatedShape = new AMShapeData();
	rotatedShape->copy(shape);

	if(rotatedShape->rotation() != 0)
	{
		rotatedShape = applySpecifiedRotation(rotatedShape,ZAXIS);
	}
	if(rotatedShape->tilt() != 0)
	{
		rotatedShape = applySpecifiedRotation(rotatedShape,XAXIS);
	}
	if(rotatedShape->yAxisRotation() != 0)
	{
		rotatedShape = applySpecifiedRotation(rotatedShape,YAXIS);
	}

	QPolygonF newShape;
	for(int i = 0; i < rotatedShape->count(); i++)
	{
		newShape<<camera_->transform3Dto2D(rotatedShape->coordinate(i));
	}

	if(distortion_)
	{
		newShape = applyDistortion(newShape);
	}

	return newShape;
}

/// applies distortion to the shape
QPolygonF AMSampleCamera::applyDistortion(QPolygonF shape) const
{
	QPolygonF polygon;
	for(int i = 0; i < shape.count(); i++)
	{
		polygon<<camera_->distortPoint(shape[i]);
	}
	return polygon;
}

/// removes distortion from the shape
QPolygonF AMSampleCamera::removeDistortion(QPolygonF shape) const
{
	QPolygonF polygon;
	for(int i = 0; i < shape.count(); i++)
	{
		polygon<<undistortPoint(shape[i]);
	}
	return polygon;
}

/// removes distortion from the point
QPointF AMSampleCamera::undistortPoint(QPointF point) const
{
	return camera_->undistortPoint(point);
}

/// moves all the shapes based on change in motor movement
void AMSampleCamera::motorMovement(double x, double y, double z, double r)
{
	QVector3D newPosition(x,y,z);
	QVector3D shift = newPosition - motorCoordinate_;
	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("AMSampleCamera::motorMovement centre of rotation is %1 %2 %3").arg(centerOfRotation_.x()).arg(centerOfRotation_.y()).arg(centerOfRotation_.z()) );

	// This seems to work a lot better, switching the sign of the y-component when we change from +r to -r
	QVector3D effectiveRotationalOffset = rotationalOffset();

	QVector3D centerOfRotation = newPosition + effectiveRotationalOffset;
	QVector3D directionOfRotation = QVector3D(0,0,1);
	centerOfRotation_ = centerOfRotation;
	directionOfRotation_ = directionOfRotation;
	AMAngle rotation (r); // default is degrees


	for(int i = 0; i <= index_; i++)
	{
		if(shapeList_[i] != beamMarkers_[0] && shapeList_[i] != beamMarkers_[1] && shapeList_[i] != beamMarkers_[2])
		{
			shiftCoordinates(shift,i);
			applyMotorRotation(i, rotation.radians());
			updateShape(i);
		}
	}
	if(samplePlateSelected_)
	{
		shiftCoordinates(shift,samplePlateShape_);
		samplePlateShape_->setCoordinateShape(applyMotorRotation(samplePlateShape_, rotation.radians()));
		updateShape(samplePlateShape_);
	}

	oldRotation_->setAngle(rotation);


}

/// applies 3D rotation and tilt to the 3D shape in an AMShapeData
QVector<QVector3D> AMSampleCamera::rotateShape(const AMShapeData *shape) const
{
	AMShapeData* newShape = new AMShapeData();
	newShape->copy(shape);
	if(newShape->rotation() != 0) newShape = applySpecifiedRotation(newShape, ZAXIS);
	if(newShape->tilt() != 0) newShape = applySpecifiedRotation(newShape, XAXIS);
	if(newShape->yAxisRotation() != 0) newShape = applySpecifiedRotation(newShape, YAXIS);
	QVector<QVector3D> returnShape;
	for(int i = 0; i < newShape->count(); i++)
	{
		returnShape<<newShape->coordinate(i);
	}
	return returnShape;
}

/// transforms a coordinate point for display on the screen
QPointF AMSampleCamera::coordinateTransform(QPointF coordinate) const
{

	QRectF activeRect = QRectF(QPointF((viewSize().width()-scaledSize().width())/2,videoTop_),
				   //					   (viewSize().height()-scaledSize().height())/2),
				   scaledSize());

	QPointF fixedCoordinate;
	qreal xCoord = activeRect.left() + coordinate.x()*activeRect.width();
	qreal yCoord = activeRect.top() + coordinate.y()*activeRect.height();
	fixedCoordinate.setX(xCoord);
	fixedCoordinate.setY(yCoord);
	return fixedCoordinate;
}


QPolygonF AMSampleCamera::screenShape(QPolygonF shape) const
{
	QPolygonF newShape;
	for(int i = 0; i < shape.count(); i++)
	{
		newShape<<coordinateTransform(shape.at(i));
	}
	return newShape;
}

/// constructs a rectangle given the desired top and bottom corners
QPolygonF AMSampleCamera::constructRectangle(QPointF topLeft, QPointF bottomRight) const
{
	QPolygonF polygon;
	QPointF topRight(bottomRight.x(),topLeft.y());
	QPointF bottomLeft(topLeft.x(),bottomRight.y());
	polygon<<topLeft<<topRight<<bottomRight<<bottomLeft<<topLeft;
	return polygon;

}

/// returns the size of a rectangle with the given points
QSizeF AMSampleCamera::size(QPointF topLeft, QPointF bottomRight) const
{
	QSizeF size;
	QPointF difference = bottomRight - topLeft;
	size.setHeight(difference.y());
	size.setWidth(difference.x());
	return size;
}

/// Checks if the shape contains the point
bool AMSampleCamera::contains(QPointF position, int index) const
{
	return shapeList_[index]->shape()->containsPoint(position, Qt::OddEvenFill);
}

/// finds the depth of the given coordinate, returns the length of depthVector
double AMSampleCamera::depth(QVector3D coordinate) const
{
	QVector3D depth = depthVector(coordinate);
	return depth.length();
}

/** returns the depth vector
/// the depth vector gives the distance from the center of the camera to an object the camera is centered on.
/// depth vector is the same for all coordinates on the plane that is normal to depth vector, with the same distance */
QVector3D AMSampleCamera::depthVector(QVector3D coordinate) const
{
	QVector3D cameraCenterClose = camera_->transform2Dto3D(QPointF(0.5,0.5),1);// actual camera center
	QVector3D cameraCenterFar = camera_->transform2Dto3D(QPointF(0.5,0.5),2);// a second vector in the direction of the center
	QVector3D cameraDirection = cameraCenterFar - cameraCenterClose;
	cameraDirection.normalize();
	QVector3D cameraToCoordinate = coordinate - cameraCenterClose;
	double length = dot(cameraToCoordinate, cameraDirection) + 1;
	QVector3D depth = length * cameraDirection;
	return depth;
}

/// finds the intersection between the beam and the shape with given index
QVector<QVector3D> AMSampleCamera::findIntersectionShape(int index, bool *isFullyWithinSample) const
{
	return findIntersectionShape(shapeList_[index], true, isFullyWithinSample);
}

QVector<QVector3D> AMSampleCamera::findIntersectionShape(const AMShapeData* shapeToIntersect, bool boundIntersection, bool *isFullyWithinSample) const
{
	/// First find the shape on the same plane
	// get the actual, rotated shape
	QVector<QVector3D> rotatedShape = rotateShape(shapeToIntersect);
	QVector<QVector3D> shape;
	QVector3D topLeft = rotatedShape.at(TOPLEFT);
	// get the height and width unit vectors of the shape
	QVector3D hHat = getHeightNormal(shapeToIntersect);
	QVector3D wHat = getWidthNormal(shapeToIntersect);
	// cross the height and width to get the normal unit vector
	QVector3D nHat = QVector3D::normal(wHat,hHat);
	// cross the width and normal to get height, in case height and width are not at right angle
	hHat = QVector3D::normal(nHat,wHat);
	int count = beamModel_->count();
	QVector<QVector3D> l0;
	l0.resize(count);
	QVector<QVector3D> lHat;
	lHat.resize(count);
	double distance[count];
	double numerator;
	double denominator;
	/// Find the shape of the beam on the plane of the shape
	/// Done by projecting each ray until it hits the plane
	/// then build a shape out of the points
	for(int i = 0; i < count; i++)
	{
		l0[i] = beamModel_->ray(i).at(0);
		lHat[i] = beamModel_->ray(i).at(1) - l0[i];
		lHat[i].normalize();
		numerator = dot((topLeft - l0[i]),nHat);
		denominator = dot(lHat[i],nHat);
		if(0 == denominator)
		{
			distance[i] = 1;
		}
		else
		{
			distance[i] = numerator/denominator;
		}
		shape<<l0[i]+lHat[i]*distance[i];
	}

	/// need to find if the two shapes have any overlap
	/// transpose to h,w,n coordinates, and throw away the n part, to get two polygons
	// h component for each shape = h^(dot)coordinate
	QPolygonF originalShape;
	QPolygonF beamShape;
	double beamHComponent [count];
	double beamWComponent [count];
	int shapeCount = rotatedShape.count();
	double shapeHComponent [shapeCount];
	double shapeWComponent [shapeCount];
	double shapeNComponent [shapeCount];
	int totalCount = std::max(shapeCount,count);
	for(int i = 0; i < totalCount; i++)
	{
		if(i<count)
		{
			beamHComponent[i] = dot(hHat,shape[i]);
			beamWComponent[i] = dot(wHat,shape[i]);

			// 2d beam shape
			beamShape<<QPointF(beamWComponent[i],beamHComponent[i]);
		}

		if(i<shapeCount)
		{
			shapeHComponent[i] = dot(hHat,rotatedShape.at(i));
			shapeWComponent[i] = dot(wHat,rotatedShape.at(i));
			// need to save the n component for returning to 3d
			shapeNComponent[i] = dot(nHat,rotatedShape.at(i));

			// 2d sample shape
			originalShape<<QPointF(shapeWComponent[i],shapeHComponent[i]);
		}
	}

	QPolygonF intersection;
	if(boundIntersection)
	{
		// take only the part that lies on the shape
		intersection = originalShape.intersected(beamShape);
		if(beamShape == intersection && isFullyWithinSample)
		{
			*isFullyWithinSample = true;
		}
		else if(isFullyWithinSample)
		{
			*isFullyWithinSample = false;
		}
	}
	else
	{
		intersection = beamShape;
	}
	if(intersection.isEmpty()) return QVector<QVector3D>();

	QVector3D point;
	QPointF oldPoint;
	QVector<QVector3D> intersectionShape;

	/// create the 3D shape by placing back on the plane
	for(int i = 0; i < intersection.count(); i++)
	{
		oldPoint = intersection.at(i);
		/// the 3D position is x*wHat + y*hHat + n*nHat
		// n is constant for all points, since it is on a plane
		point = oldPoint.x()*wHat + oldPoint.y()*hHat + shapeNComponent[i%shapeCount]*nHat;
		intersectionShape<<point;
	}

	return intersectionShape;
}

/// returns true if beam is cut off by all shapes. Returns false if beam is fully within one shape
bool AMSampleCamera::isBeamCutOff()
{
	return beamCutOff_;
}

/// returns the QPolygonF of a set of QVector3D's that can be directly displayed on the screen
QPolygonF AMSampleCamera::intersectionScreenShape(QVector<QVector3D> shape3D) const
{

	AMShapeData* newShape = new AMShapeData();
	newShape->setCoordinateShape(shape3D);
	newShape->setRotation(0);
	newShape->setTilt(0);
	newShape->setYAxisRotation(0);


	QPolygonF shape;
	shape = subShape(newShape);
	/// \todo delete newShape

	shape = screenShape(shape);

	return shape;

}

/// finds the coordinate of the center of the shape
QVector3D AMSampleCamera::centerCoordinate(int index) const
{
	return shapeList_[index]->centerCoordinate();
}



/// returns the dot product of two vectors
double AMSampleCamera::dot(QVector3D a, QVector3D b) const
{
	double value = QVector3D::dotProduct(a,b);
	return value;
}


QVector3D AMSampleCamera::getHeightNormal(const AMShapeData* shape) const
{
	QVector<QVector3D> rotatedShape = rotateShape(shape);
	QVector3D heightVector = rotatedShape.at(shape->count()-2) - rotatedShape.at(0);
	return heightVector.normalized();
}

QVector3D AMSampleCamera::getWidthNormal(const AMShapeData* shape) const
{
	QVector<QVector3D> rotatedShape = rotateShape(shape);
	QVector3D widthVector = rotatedShape.at(1) - rotatedShape.at(0);
	return widthVector.normalized();
}

/// gets the normal vector pointing out of a shape
QVector3D AMSampleCamera::getNormal(QVector3D heightVector, QVector3D widthVector) const
{
	return QVector3D::normal(heightVector,widthVector);
}

/// overload of getNormal, takes an AMShapeData
QVector3D AMSampleCamera::getNormal(const AMShapeData* shape) const
{
	return getNormal(getHeightNormal(shape),getWidthNormal(shape));
}

/// finds the point on the shape that is being drawn on (probably the sample plate)
QVector3D AMSampleCamera::getPointOnShape(const QPointF &position, const QVector3D &nHat, bool useOffset) const
{
	return getPointOnShape(drawOnShape_, position, nHat, useOffset);
}

/// finds the point on a specified shape
QVector3D AMSampleCamera::getPointOnShape(const AMShapeData *shape, const QPointF &position, const QVector3D &nHat, bool useOffset) const
{
	/// figure out line
	/// pick a depth to use, doesn't really matter what it is
	double depth = camera_->focalLength();
	QVector3D l0 = camera_->transform2Dto3D(position,depth);
	QVector3D lVector = camera_->transform2Dto3D(position,2*depth) - l0;
	/// got the line, now do some geometry
	QVector<QVector3D> rotatedShape = rotateShape(shape);
	double offsetCoefficient = 0;
	if(useOffset)
		offsetCoefficient = 1.0;
	// if using offset, use a point that is offset from the actual shape
	double numerator = dot(((rotatedShape.at(TOPLEFT)+(offsetCoefficient*sampleOffset_*nHat)) - l0), nHat);
	double denominator = dot(lVector,nHat);
	double distance = 0;
	if(denominator == 0)
	{
		if(numerator == 0 && !debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_GETPOINTONSHAPE_LINE_IS_ON_PLANE, QString("A call was made to getPointOnShape and the line is on the plane.") );
		else if (!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_GETPOINTONSHAPE_LINE_IS_PARALLEL_TO_PLANE, QString("A call was made to getPointOnShape and the line is parallel to the plane.") );
	}
	else
	{
		distance = numerator/denominator;
	}
	QVector3D pointOnShape = l0 + distance*lVector;
	return pointOnShape;
}

QVector3D AMSampleCamera::getPointOnShape(const AMShapeData* shape, const QPointF &position, bool useOffset) const
{
	return getPointOnShape(shape,position,getNormal(shape), useOffset);
}

QVector3D AMSampleCamera::downVector() const
{
	QVector3D top = camera_->transform2Dto3D(QPointF(0.5,0),1);
	QVector3D bottom = camera_->transform2Dto3D(QPointF(0.5,1),1);
	QVector3D downVector = bottom - top;
	return downVector;
}

QVector3D AMSampleCamera::rightVector() const
{
	QVector3D left = camera_->transform2Dto3D(QPointF(0,0.5),1);
	QVector3D right = camera_->transform2Dto3D(QPointF(1,0.5),1);
	QVector3D rightVector = right - left;
	return rightVector;
}

/// Inserts an AMShapeData into the model and connects the necessary signals
void AMSampleCamera::insertItem(AMShapeData *item)
{
	index_++;
	beginInsertRows(QModelIndex(),index_,index_);
	shapeList_<<item;
	endInsertRows();
	connect(item, SIGNAL(nameChanged(QString)), this, SIGNAL(shapeNameChanged(QString)));
	connect(item, SIGNAL(otherDataFieldOneChanged(QString)), this, SIGNAL(otherDataOneChanged(QString)));
	connect(item, SIGNAL(otherDataFieldTwoChanged(QString)), this, SIGNAL(otherDataTwoChanged(QString)));
	connect(item, SIGNAL(shapeDataChanged(AMShapeData*)), this, SLOT(updateShape(AMShapeData*)));
	connect(item, SIGNAL(shapeDataRemoved(AMShapeData*)), this, SLOT(removeShapeData(AMShapeData*)));
	updateShape(index_);
	emit shapesChanged();
}

AMShapeData *AMSampleCamera::takeItem(int index)
{
	index_--;
	beginRemoveRows(QModelIndex(),index,index);
	AMShapeData* oldShape = shapeList_.takeAt(index);
	endRemoveRows();
	emit shapesChanged();
	return oldShape;
}

/// finds the point to move the plate to intersect the beam with the given point.
/// find corner determines whether to match beam corner to point (true) or beam centre to point(false)
QVector3D AMSampleCamera::beamIntersectionPoint(QVector3D samplePoint, bool findCorner)
{
	// first find the plane that the clicked point lies on
	// this will be an x-z plane with constant y (as it will only move with x,z motors)
	// then find where the beam intersects this plane.
	double y = samplePoint.y();
	QVector3D beamSpot(0,0,0);
	AMShapeData* newShape = new AMShapeData();
	QVector<QVector3D> coordinateShape;
	coordinateShape<<QVector3D(0,0,0)<<QVector3D(0,0,0)<<QVector3D(0,0,0)<<QVector3D(0,0,0);
	newShape->setCoordinateShape(coordinateShape);
	QVector3D topLeftBeamIntersectionPoint;
	for(int i = 0; i < beamConfiguration()->count()-1; i++)
	{
		// equation for line:
		//			x = x0 + at
		//			y = y0 + bt
		//			z = z0 + ct
		// find abc for xyz
		QVector3D pointOne = beamConfiguration()->ray(i).first();
		QVector3D pointTwo = beamConfiguration()->ray(i).last();
		double x0 = pointOne.x();
		double y0 = pointOne.y();
		double z0 = pointOne.z();

		double xOne = pointTwo.x();
		double yOne = pointTwo.y();
		double zOne = pointTwo.z();

		double at = xOne-x0;
		double bt = yOne-y0;
		double ct = zOne-z0;

		// find parameter
		double t = (pointTwo - pointOne).length();

		// find relative direction lengths
		double a = at/t;
		double b = bt/t;
		if(b == 0)
		{
			if(!debuggingSuppressed_)
				AMErrorMon::debug(this, AMSAMPLECAMERA_BEAMINTERSECTIONPOINT_RAY_DOES_NOT_MOVE_IN_Y, QString("Ray does not move in y direction, cannot intersect it.") );
			return samplePoint;
		}
		double c = ct/t;

		// find the point at the desired y
		double beamLength = (y - y0)/b;

		double x = x0 + a*beamLength;
		double z = z0 + c*beamLength;
		QVector3D beamPoint(x,y,z);
		newShape->setCoordinate(beamPoint,i);
		beamSpot+=beamPoint;
		if(findCorner && i == 0)
		{
			topLeftBeamIntersectionPoint = beamPoint;
		}


	}

	newShape->setName(QString("Shape %1").arg(index_));
	updateShape(newShape);
	// find the center of the point
	beamSpot /= beamConfiguration()->count()-1;
	if(findCorner)
	{
		return topLeftBeamIntersectionPoint;
	}
	return  beamSpot;
}

/// moves the x, y, z motors
bool AMSampleCamera::moveMotors(double x, double y, double z)
{
	double rotation = ssaManipulatorRot_->value();
	return moveMotors(x,y,z,rotation);
}

/// moves the x, y, z, and rotation motors
bool AMSampleCamera::moveMotors(double x, double y, double z, double rotation)
{
	bool success = false;
	if(motorMovementEnabled())
	{
		const int numberOfMotors = 4;
		success = true;
		AMControl::FailureExplanation failure [numberOfMotors];
		double xMin = ssaManipulatorX_->minimumValue();
		double xMax = ssaManipulatorX_->maximumValue();
		double yMin = ssaManipulatorY_->minimumValue();
		double yMax = ssaManipulatorY_->maximumValue();
		double zMin = ssaManipulatorZ_->minimumValue();
		double zMax = ssaManipulatorZ_->maximumValue();
		double rMin = ssaManipulatorRot_->minimumValue();
		double rMax = ssaManipulatorRot_->maximumValue();
		if(x > xMax) x = xMax;
		else if(x < xMin) x = xMin;
		if(y > yMax) y = yMax;
		else if(y < yMin) y = yMin;
		if(z > zMax) z = zMax;
		else if(z < zMin) z = zMin;
		if(rotation > rMax) rotation = rMax;
		else if (rotation < rMin) rotation = rMin;
		failure[0] = ssaManipulatorX_->move(x);
		failure[1] = ssaManipulatorY_->move(y);
		failure[2] = ssaManipulatorZ_->move(z);
		failure[3] = ssaManipulatorRot_->move(rotation);

		for(int i = 0; i < numberOfMotors; i++)
		{
			if(failure[i] != AMControl::NoFailure)
			{
				success = false;
				if(!debuggingSuppressed_)
					AMErrorMon::debug(this, AMSAMPLECAMERA_MOVEMOTORS_FAILURE, QString("Motors failed to move and reported an error: %1").arg(AMControl::failureExplanation(failure[i])) );
			}
		}
	}
	return success;
}

/// Calculates the coordinate of each point in "points"
/// "Coordinates" is the motor location corresponding to each point
/// "rotations" is the motor rotation corresponding to each point
/// "numberOfPoints" is the number of points used for calibration
QVector<QVector3D> AMSampleCamera::findSamplePlateCoordinate(const QVector<QVector3D> coordinates, const QVector<QPointF> points, const QVector<double> rotations, const int numberOfPoints) const
{
	/// This function finds the coordinate of a sample plate point as it moves through each coordinate in "coordinates"
	/// This is done using SVD decomposition to perform a least squares solution
	/// to the matrix defined below.

	// first get rid of the lens distortion on the test points.
	QVector<QPointF> adjustedPoints;
	for(int i = 0; i < numberOfPoints; i++)
	{
		adjustedPoints<<undistortPoint(points.at(i));
	}

	// find a reasonable starting point for the calculations. Use the positions of the
	// calibration points to get approximations of the actual coordinate
	QVector<double> depths;
	for(int i = 0; i < numberOfPoints; i++)
	{
		depths<<depth(coordinates.at(i));
	}

	/// Find the line along which each coordinate lies
	/// //////////////////////////////////////////////////////////////////////////////////////////
	// get two different points that could correspond to the selected point, at different depths
	// these points will be used to calculate the line along which the desired coordinate lies
	QVector<QVector3D> bases;
	QVector<QVector3D> lengths;
	QVector<QVector3D> vectors;
	int additionalDepth = 5;

	for(int i = 0; i < numberOfPoints; i++)
	{
		bases<<camera_->transform2Dto3D(adjustedPoints.at(i), depths.at(i));
		lengths<<camera_->transform2Dto3D(adjustedPoints.at(i), depths.at(i) + additionalDepth);
		vectors<<(lengths.at(i)-bases.at(i)).normalized();

	}

	/// //////////////////////////////////////////////////////////////////////////////////////////



	/// How to solve this problem:
	/// basically have a fairly simple geometric problem -
	/// with rotation = 0. Shift between the two known coordinates
	/// is equivalent to shift between two desired coordinates (as they are the
	/// same point, after movement).
	/// with rotation != 0, the vector between the two desired coordinates is
	/// shift + L, where L is the vector that accounts for the difference due to rotation
	/// we get this by the following:

	/// For each point we have two equations in three dimensions:
	/// Pn -tnvn = Bn [1]
	/// Pn - ln = CoRn
	/// Pn is the coordinate at motor position n
	/// tnvn is the vector from Bn to Pn
	/// ln is the vector from CoRn to Pn
	/// CoRn is the centre of rotation at motor position n

	/// Bn is determined above ("bases.at(n)")
	/// CoRn is simply motor rotation + rotation offset ("coordinates.at(n) + rotationalOffset()")
	/// vn is determined above ("vectors.at(n)")
	/// ln is determined by rotating l0, which must be solved for
	/// each ln is given by ln = rotation(l0,rotations.at(n)-rotations.at(0))
	/// where rotation is a general three-dimensional rotation matrix

	/// constructing these equations in a matrix we get ~
	///	A =	[	I	|	V
	///			I	|	R	]
	/// where I is a 3nx3n identity matrix, V is the 3nx(n+3) vector matrix
	/// containing the each vector from "vectors" in the first n
	/// columns, with each element on its own row
	/// [ v0.x 0    0 0 0 ...
	///	  v0.y 0    0 0 0 ...
	///	  v0.z 0    0 0 0 ...
	///   0	   v1.x 0 0 0 ...
	///	  .    .    . . . ...]
	/// R is the 3nx(n+3) matrix containing all the rotations in the last three columns
	/// [ 0 0 0 a0 b0 c0
	///	  0 0 0 d0 e0 f0
	///   0 0 0 g0 h0 i0
	///	  0 0 0 a1 b1 c1
	///		etc.

	/// We also place the solutions to these equations in a matrix, B
	/// B = [B0,...,Bm,CoR0,...CoRm]^T
	/// Then solve Ax=B for x
	/// This is done using the SVD decomposition


	QVector<QVector3D> centreOfRotations;
	QVector3D unitRotation;
	QVector<AMAngle *> referenceRotations;
	QVector<QVector3D> lnx;
	QVector<QVector3D> lny;
	QVector<QVector3D> lnz;

	for(int i = 0; i < numberOfPoints; i++)
	{
		referenceRotations<<new AMAngle(rotations.at(i)-rotations.first(), AMAngle::DEG);// assign in degrees
	}

	unitRotation = directionOfRotation_.normalized();

	for(int i = 0; i < numberOfPoints; i ++)
	{
		centreOfRotations<<coordinates.at(i)+rotationalOffset();
	}

	for(int i = 0 ; i < numberOfPoints; i++)
	{
		lnx<<findRotationMatrixXRow(unitRotation,referenceRotations.at(i)->radians());
		lny<<findRotationMatrixYRow(unitRotation,referenceRotations.at(i)->radians());
		lnz<<findRotationMatrixZRow(unitRotation,referenceRotations.at(i)->radians());
	}
	/// delete the angles when done with them
	foreach(AMAngle *angle, referenceRotations)
	{
		delete angle;
	}



	MatrixXd coeffMatrix = constructSampleCoefficientMatrix(vectors,lnx,lny,lnz, numberOfPoints);

	QString row = "";
	for(int i = 0; i < coeffMatrix.rows(); i++)
	{
		for(int j = 0; j < coeffMatrix.cols(); j++)
		{
			row.append(QString("%1 ").arg(coeffMatrix(i,j)));
		}
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, row );
		row = "";
	}

	MatrixXd solutionMatrix (6*numberOfPoints,1);
	for(int i = 0; i < solutionMatrix.rows(); i++)
	{
		int index;
		QVector3D element;
		if(i < 3*numberOfPoints)
		{
			index = i/3;
			element = bases.at(index);

		}
		else
		{
			index = i/3 - numberOfPoints;
			element = centreOfRotations.at(index);
		}
		int section = i%3;
		switch(section)
		{
		case 0:
			solutionMatrix(i,0) = element.x();
			break;
		case 1:
			solutionMatrix(i,0) = element.y();
			break;
		case 2:
			solutionMatrix(i,0) = element.z();
			break;
		}
	}

	row = "";
	for(int i = 0; i < solutionMatrix.rows(); i++)
	{
		for(int j = 0; j < solutionMatrix.cols(); j++)
		{
			row.append(QString("%1 ").arg(solutionMatrix(i,j)));
		}
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, row );
		row = "";
	}

	MatrixXd parameters = computeSVDLeastSquares(coeffMatrix,solutionMatrix);


	// Debugging output
	MatrixXd sampleSolution = coeffMatrix*parameters;
	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Solution should be: ") );
	if(!debuggingSuppressed_)
	{
		for(int i = 0; i < sampleSolution.rows(); i++)
		{
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("SampleSolution(i) %1 SolutionMatrix(i) %2 Error %3").arg(sampleSolution(i)).arg(solutionMatrix(i)).arg(100*(solutionMatrix(i)-sampleSolution(i))/solutionMatrix(i)) );
		}
	}


	if(!debuggingSuppressed_)
	{
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("X Matrix:") );
		for(int i = 0; i < parameters.rows(); i++)
		{
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("%1").arg(parameters(i)) );
		}
	}
	QVector<QVector3D> solutions;
	for(int i = 0; i < numberOfPoints; i++)
	{
		solutions<<QVector3D(parameters(3*i),parameters(3*i+1),parameters(3*i+2));
	}

	/// The last element of solutions should be the coordinate corresponding to the current motor coordinates

	return solutions;



}

/// construct the coefficient matrix for solving for the sample plate position
Eigen::MatrixXd AMSampleCamera::constructSampleCoefficientMatrix(const QVector<QVector3D> vectors, const QVector<QVector3D> rotationX, const QVector<QVector3D> rotationY, const QVector<QVector3D> rotationZ, const int numberOfPoints) const
{

	/// constructs the matrix of the form
	/// Pn - vn
	/// ...
	/// Pn - ln
	/// ...
	MatrixXd coefficientMatrix = MatrixXd::Zero(6*numberOfPoints,4*numberOfPoints+3);

	for(int i = 0; i < coefficientMatrix.rows(); i++)
	{
		for(int j = 0; j < coefficientMatrix.cols(); j++)
		{
			if(i == j && i < 3*numberOfPoints)
			{
				coefficientMatrix(i,j) = 1;
			}
			else if (j+3*numberOfPoints == i)
			{
				coefficientMatrix(i,j) = 1;
			}
			else if (i < 3*numberOfPoints)
			{
				if(j == (3*numberOfPoints + i/3))
				{
					int index = i/3;
					int section = i%3;

					switch(section)
					{
					case 0:
						coefficientMatrix(i,j) = -1*vectors.at(index).x();
						break;
					case 1:
						coefficientMatrix(i,j) = -1*vectors.at(index).y();
						break;
					case 2:
						coefficientMatrix(i,j) = -1*vectors.at(index).z();
						break;
					}
				}

			}
			else if (i >= 3*numberOfPoints && j >= 4*numberOfPoints)
			{
				int index = (i-3*numberOfPoints)/3;
				int section = i%3;
				int col = j - 4*numberOfPoints;
				QVector3D currentRotationRow;
				switch(section)
				{
				case 0:
					currentRotationRow = rotationX.at(index);
					break;
				case 1:
					currentRotationRow = rotationY.at(index);
					break;
				case 2:
					currentRotationRow = rotationZ.at(index);
					break;
				}
				switch(col)
				{
				case 0:
					coefficientMatrix(i,j) = -1*currentRotationRow.x();
					break;
				case 1:
					coefficientMatrix(i,j) = -1*currentRotationRow.y();
					break;
				case 2:
					coefficientMatrix(i,j) = -1*currentRotationRow.z();
					break;
				}

			}

		}
	}

	if(!debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("First rotation is %1").arg(rotationX.at(0).x()) );

	return coefficientMatrix;
}


/// find the first row of a general rotation matrix
/// rotationVector must be a unit vector and
/// angleOfRotation must be in radians
QVector3D AMSampleCamera::findRotationMatrixXRow(QVector3D rotationVector, double angleOfRotation) const
{
	QVector3D rotationParameter;
	rotationParameter.setX(cos(angleOfRotation) + pow(rotationVector.x(),2)*(1-cos(angleOfRotation)));
	rotationParameter.setY(rotationVector.x()*rotationVector.y()*(1-cos(angleOfRotation)) - rotationVector.z()*sin(angleOfRotation));
	rotationParameter.setZ(rotationVector.x()*rotationVector.z()*(1-cos(angleOfRotation)) + rotationVector.y()*sin(angleOfRotation));
	return rotationParameter;
}

/// find the second row of a general rotation matrix
/// rotationVector must be a unit vector and
/// angleOfRotation must be in radians
QVector3D AMSampleCamera::findRotationMatrixYRow(QVector3D rotationVector, double angleOfRotation) const
{
	QVector3D rotationParameter;
	rotationParameter.setX(rotationVector.x()*rotationVector.y()*(1-cos(angleOfRotation)) + rotationVector.z()*sin(angleOfRotation));
	rotationParameter.setY(cos(angleOfRotation) + pow(rotationVector.y(),2)*(1-cos(angleOfRotation)));
	rotationParameter.setZ(rotationVector.y()*rotationVector.z()*(1-cos(angleOfRotation)) - rotationVector.x()*sin(angleOfRotation));
	return rotationParameter;
}

/// find the third row of a general rotation matrix
/// rotationVector must be a unit vector and
/// angleOfRotation is in radians
QVector3D AMSampleCamera::findRotationMatrixZRow(QVector3D rotationVector, double angleOfRotation) const
{
	QVector3D rotationParameter;
	rotationParameter.setX(rotationVector.x()*rotationVector.z()*(1-cos(angleOfRotation)) - rotationVector.y()*sin(angleOfRotation));
	rotationParameter.setY(rotationVector.y()*rotationVector.z()*(1-cos(angleOfRotation)) + rotationVector.x()*sin(angleOfRotation));
	rotationParameter.setZ(cos(angleOfRotation) + pow(rotationVector.z(),2)*(1-cos(angleOfRotation)));
	return rotationParameter;
}

/// constructs a 3x3 general rotation matrix
MatrixXd AMSampleCamera::constructRotationMatrix(QVector3D rotationVector, double angleOfRotation) const
{
	MatrixXd rotationMatrix (3,3);
	QVector3D rotationMatrixX = findRotationMatrixXRow(rotationVector,angleOfRotation);
	QVector3D rotationMatrixY = findRotationMatrixYRow(rotationVector,angleOfRotation);
	QVector3D rotationMatrixZ = findRotationMatrixZRow(rotationVector,angleOfRotation);

	rotationMatrix<<	rotationMatrixX.x(),rotationMatrixX.y(), rotationMatrixX.z(),
			rotationMatrixY.x(),rotationMatrixY.y(), rotationMatrixY.z(),
			rotationMatrixZ.x(),rotationMatrixZ.y(), rotationMatrixZ.z();
	return rotationMatrix;
}

/// returns a base point and a unit vector that defines a line that appears at point "point" on the camera
QPair<QVector3D, QVector3D> AMSampleCamera::findScreenVector(QPointF point, QVector3D referenceCoordinate, QVector3D optionalReference)
{
	QVector3D basePoint = camera_->transform2Dto3D(point,depth(referenceCoordinate));
	QVector3D otherPoint = camera_->transform2Dto3D(point, depth(optionalReference));
	QVector3D direction = otherPoint - basePoint;
	direction.normalize();
	QPair<QVector3D,QVector3D> result;
	result.first = basePoint;
	result.second = direction;
	return result;

}

/// construct the matrix used for determining the centre of rotation
MatrixXd AMSampleCamera::constructCentreOfRotationMatrix(const QVector<AMAngle *> &rotations, const QVector<QVector3D> &vectors, const int &numberOfPoints)
{
	const int dimensions = 3;
	QVector3D rotationVector = directionOfRotation_;

	QVector<QVector<QVector3D> > rotationRows;
	for(int i = 0; i < numberOfPoints; i++)
	{
		QVector<QVector3D> newVector;
		newVector<<findRotationMatrixXRow(rotationVector,rotations.at(i)->radians());
		newVector<<findRotationMatrixYRow(rotationVector,rotations.at(i)->radians());
		newVector<<findRotationMatrixZRow(rotationVector,rotations.at(i)->radians());
		rotationRows<<newVector;
	}

	int rows = dimensions * numberOfPoints;
	int cols = dimensions + numberOfPoints;

	MatrixXd matrix = MatrixXd::Zero(rows,cols);

	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
		{
			int index = i/3;
			int subrow = i%3;
			int element = j%3;
			if(j < dimensions)
			{
				int offsetVal = 0;
				if(i%dimensions == j%dimensions)
				{
					offsetVal = 1;
				}

				QVector3D rotationSection = rotationRows.at(index).at(subrow);
				switch(element)
				{
				case 0:
					matrix(i,j) = rotationSection.x() - offsetVal;
					break;
				case 1:
					matrix(i,j) = rotationSection.y() - offsetVal;
					break;
				case 2:
					matrix(i,j) = rotationSection.z() - offsetVal;
					break;
				}
			}
			else
			{
				if(j-dimensions == index)
				{
					switch(subrow)
					{
					case 0:
						matrix(i,j) = -1*vectors.at(index).x();
						break;
					case 1:
						matrix(i,j) = -1*vectors.at(index).y();
						break;
					case 2:
						matrix(i,j) = -1*vectors.at(index).z();
						break;
					}
				}
			}
		}
	}

	return matrix;
}

/// construct the solution matrix for the centre of rotation (right hand side)
MatrixXd AMSampleCamera::constructCentreOfRotationCoordinateMatrix(const QVector<QVector3D> &coordinates, QVector<QVector3D> bases, const int &numberOfPoints)
{
	const int dimensions = 3;
	int rows = dimensions * numberOfPoints;
	int cols = 1; // row matrix, it is the solution
	MatrixXd matrix(rows,cols);

	for(int j = 0; j < matrix.cols(); j++)
	{
		for(int i = 0; i < matrix.rows(); i++)
		{
			int index = i/3;
			int element = i%3;
			switch(element)
			{
			case 0:
				matrix(i,j) = bases.at(index).x() - coordinates.at(index).x();
				break;
			case 1:
				matrix(i,j) = bases.at(index).y() - coordinates.at(index).y();
				break;
			case 2:
				matrix(i,j) = bases.at(index).z() - coordinates.at(index).z();
				break;
			}
		}
	}
	return matrix;
}

/// Solve the centre of rotation matrix
MatrixXd AMSampleCamera::solveCentreOfRotationMatrix(MatrixXd coeffMatrix, MatrixXd coordinateMatrix)
{
	if(coeffMatrix.rows() != coordinateMatrix.rows() && !debuggingSuppressed_)
		AMErrorMon::debug(this, AMSAMPLECAMERA_ROTATION_MATRIX_COUNT_MISMATCH, QString("Cannot solve center of rotation matrix, rows of matrices do not match. Coefficient row count: %1 Coordinate row count: %2").arg(coeffMatrix.rows()).arg(coordinateMatrix.rows()) );

	MatrixXd solution = computeSVDLeastSquares(coeffMatrix,coordinateMatrix);
	return solution;
}


/// computes the SVD least squares solution
MatrixXd AMSampleCamera::computeSVDLeastSquares(MatrixXd A, MatrixXd Y) const
{
	return camera_->computeSVDLeastSquares(A,Y);
}

/// computes the SVD homogenous solution for the largest singular value
MatrixXd AMSampleCamera::computeSVDHomogenous(MatrixXd leftHandSide) const
{
	// solution is first column of V matrix - solution for maximum singular value of m
	return camera_->computeSVDHomogenous(leftHandSide);
}

/// converts a vector to a matrix.  Matrix is a column vector by default, but
/// a row vector may be passed in instead
MatrixXd AMSampleCamera::vectorToMatrix(QVector3D vector, MatrixXd matrix) const
{
	matrix<< vector.x(),vector.y(),vector.z();
	return matrix;
}

/// copies the first three elements of a matrix into a vector
/// there must be at least 3 rows or 3 columns in the matrix
QVector3D AMSampleCamera::matrixToVector(MatrixXd matrix) const
{
	// need at least three rows or columns
	if(matrix.rows() < 3 && matrix.cols() < 3)
	{
		return QVector3D(0,0,0);
	}
	return QVector3D(matrix(0), matrix(1), matrix(2));
}

/// prints out a matrix for debuggging.
/// not always consistent about negatives
void AMSampleCamera::debugPrintMatrix(const Eigen::MatrixXd matrix) const
{
	QString row = "";
	for(int i = 0; i < matrix.rows(); i++)
	{
		for(int j = 0; j < matrix.cols(); j++)
		{
			row.append(QString("%1\t").arg(matrix(i,j),1,'f',4));
		}
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("%1").arg(row) );
		row = "";
	}

}

/// set if the beam is not fully within a sample
void AMSampleCamera::setBeamCutOff(bool beamCutOff)
{
	beamCutOff_ = beamCutOff;
}

/// determines if any samples overlap
bool AMSampleCamera::samplesOverlap()
{
	for(int i = 0; i < shapeList().length(); i++)
	{
		for(int j = i+1; j < shapeList().length(); j++)
		{
			if(!screenShape(*shapeList().at(i)->shape()).intersected(screenShape(*shapeList().at(j)->shape())).isEmpty())
			{
				return true;
			}
		}
	}
	return false;
}

/// causes samples and sample plate to block the beam as it goes
void AMSampleCamera::blockBeam()
{

	if(intersectionShapes_.isEmpty())
	{
		return;
	}

	QVector3D beamDirection = beamModel_->ray(0).at(1) - beamModel_->ray(0).at(0);
	beamDirection.normalize();
	int count = intersectionShapes_.count();
	QMap<double, int> *distances = new QMap<double, int>;


	/// Now order all the intersections based on direction
	int referenceIndex = 0;

	for(int i = 0; i < count; i++)
	{
		double distance = dot(intersectionShapes_.at(i).at(0) - intersectionShapes_.at(referenceIndex).at(0), beamDirection);
		if(!distances->contains(distance))
			distances->insert(distance, i);
	}

	QMap<double, int>::const_iterator i = distances->constBegin();
	QVector<QVector3D> usedBeam = intersectionShapes_[i.value()];
	i++;

	while(i != distances->constEnd())
	{
		QVector<QVector3D> currentIntersection = intersectionShapes_[i.value()];
		QVector<QVector3D> projectedUsedBeam;
		/// project used beam to current intersection plane
		// point of intersection is the point
		// la+(lb-la)t
		// la is the point on the usedBeam shape, la is that +beamDirection
		// t is found by solving
		//[ t ]   [ xa-xb x1-x0 x2-x0 ]-1 [ xa-x0 ]
		//[ u ] = [ ya-yb y1-y0 y2-y0 ]   [ ya-y0 ]
		//[ v ]   [ za-zb z1-z0 z2-z0 ]   [ za-z0 ]
		// Xa = la
		// Xb = lb
		// X0, X1, X2 are points on the plane (ie.points on the current intersection shape)
		// each column represents a particular vector:
		// a-b = -1*beamDirection
		// 1-0 = currentIntersection[1] - currentIntersection[0]
		// 2-0 same as above, but 2
		// a - 0, same but current used beam point - point 0

		for(int j = 0; j < usedBeam.count(); j++)
		{
			double t;
			QVector3D ab = -1*beamDirection;
			QVector3D x1x0 = currentIntersection.at(1) - currentIntersection.at(0);
			QVector3D x2x0 = currentIntersection.at(2) - currentIntersection.at(0);
			QVector3D ax0 = usedBeam.at(j) - currentIntersection.at(0);

			MatrixXd matrixOne (3,3);
			MatrixXd matrixTwo (3,1);

			matrixOne<< ab.x(), x1x0.x(), x2x0.x(),
					ab.y(), x1x0.y(), x2x0.y(),
					ab.z(), x1x0.z(), x2x0.z();

			matrixTwo<<ax0.x(),
				   ax0.y(),
				   ax0.z();
			MatrixXd solution = matrixOne.inverse()*matrixTwo;
			t = solution(0);
			projectedUsedBeam<<usedBeam.at(j) + t*beamDirection;
		}



		/// form a set of plane vectors to map to
		QVector3D vector1 = projectedUsedBeam.at(1) - projectedUsedBeam.at(0);
		QVector3D vector2;
		int p = 2;
		do
		{
			vector2 = projectedUsedBeam.at(p)-projectedUsedBeam.at(p-1);
			p++;

		} while (vector2 == vector1 && p < projectedUsedBeam.count());
		QVector3D nHat = QVector3D::crossProduct(vector1,vector2);
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Point 0: %1 %2 1: %3 %4 2:%5 %6").arg(projectedUsedBeam.at(0).x()).arg(projectedUsedBeam.at(0).y()).arg(projectedUsedBeam.at(1).x()).arg(projectedUsedBeam.at(1).y()).arg(projectedUsedBeam.at(2).x()).arg(projectedUsedBeam.at(2).y()) );
		nHat = nHat/nHat.length();
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Nhat: %1 %2 %3").arg(nHat.x()).arg(nHat.y()).arg(nHat.z()) );
		QVector3D xHat = (projectedUsedBeam.at(1) - projectedUsedBeam.at(0));
		xHat = xHat/xHat.length();
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Xhat: %1 %2 %3").arg(xHat.x()).arg(xHat.y()).arg(xHat.z()) );
		QVector3D yHat = QVector3D::crossProduct(nHat,xHat);
		yHat = yHat/yHat.length();
		if(!debuggingSuppressed_)
			AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Yhat: %1 %2 %3").arg(yHat.x()).arg(yHat.y()).arg(yHat.z()) );

		/// create a matrix for the transform
		MatrixXd planeMatrix (3,3);
		planeMatrix<<	xHat.x(), yHat.x(), nHat.x(),
				xHat.y(), yHat.y(), nHat.y(),
				xHat.z(), yHat.z(), nHat.z();

		debugPrintMatrix(planeMatrix);
		/// to find the plane coordinates
		///  PlaneCoords = RealCoords * planeMatrix
		/// to find the real coordinates from plane coordinates
		/// RealCoords = PlaneCoords * planeMatrix^T


		/// find the xHat, yHat components of each point, map it to xy in 2D, then get the new shapes,
		/// map xy to xHat,yHat, keep the nHat component what it was (should be the same for all points)

		QVector<QVector3D> alternateProjectedCoordinates;
		QPolygonF alternateProjectedShape;
		QVector<QVector3D> alternateIntersectionCoordinates;
		QPolygonF alternateIntersectionShape;
		/// get each point in terms of the new orthogonal components
		for(int pcount = 0; pcount < projectedUsedBeam.count(); pcount++)
		{
			QVector3D point = projectedUsedBeam.at(pcount);
			QVector3D alternatePoint;
			MatrixXd realCoordinate (1,3);

			realCoordinate = vectorToMatrix(point, realCoordinate);
			MatrixXd newPoint = realCoordinate * planeMatrix;
			alternatePoint = matrixToVector(newPoint);
			alternateProjectedCoordinates<<alternatePoint;
			alternateProjectedShape<<alternatePoint.toPointF();
		}
		for(int pcount = 0; pcount < currentIntersection.count(); pcount++)
		{
			QVector3D point = currentIntersection.at(pcount);
			QVector3D alternatePoint;
			MatrixXd realCoordinate (1,3);
			realCoordinate = vectorToMatrix(point, realCoordinate);
			alternatePoint = matrixToVector(realCoordinate*planeMatrix);
			alternateIntersectionCoordinates<<alternatePoint;
			alternateIntersectionShape<<alternatePoint.toPointF();
		}
		/// find the total blocked beam
		QPolygonF newUsedBeam = alternateProjectedShape.united(alternateIntersectionShape);
		if(!debuggingSuppressed_)
		{
			for(int x = 0, size = newUsedBeam.count(); x < size; x++)
			{
				AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("New total beam is (point %1): %2, %3").arg(x).arg(newUsedBeam.at(x).x()).arg(newUsedBeam.at(x).y()) );
			}
		}
		/// find the part of the beam that has not been blocked
		/// \todo determine why subtracted sometimes fails
		// this sometimes produces a zero shape when it should not
		QPolygonF newIntersectionShape = alternateIntersectionShape.subtracted(alternateProjectedShape);
		if(!debuggingSuppressed_)
		{
			for(int x = 0, size = alternateProjectedShape.count(); x < size; x++)
				AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Old intersected shape (point %1): %2, %3").arg(x).arg(alternateProjectedShape.at(x).x()).arg(alternateProjectedShape.at(x).y()) );
			for(int x = 0, size = alternateProjectedShape.count(); x < size; x++)
				AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Beam shape (point %1): %2, %3").arg(x).arg(alternateProjectedShape.at(x).x()).arg(alternateProjectedShape.at(x).y()) );
			for(int x = 0, size = newIntersectionShape.count(); x < size; x++)
				AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("New intersected shape (point %1): %2, %3").arg(x).arg(newIntersectionShape.at(x).x()).arg(newIntersectionShape.at(x).y()) );
		}
		double zComponent = alternateProjectedCoordinates.at(0).z();

		alternateProjectedCoordinates.clear();

		QVector<QVector3D> newUsedBeamCoords;
		QVector<QVector3D> newIntersectionCoords;

		for(int pcount = 0; pcount < newUsedBeam.count(); pcount++)
		{
			/*
			QVector3D pointOne = projectedUsedBeam.at(m);
			QVector3D pointTwo = projectedUsedBeam.at(m+1);
			for(int k = 0; k < currentIntersection.count() - 1; k++)
			{
				QVector3D currentPointOne = currentIntersection.at(k);
				QVector3D currentPointTwo = currentIntersection.at(k+1);

				/// find the intersection of the two lines, bounded by the points
				QVector3D intersectionPoint;
				/// intersection point is the point where P1+a*(P2-P1) = cP1 +b*(cP2-cP1)
				///  a and b are both <= 1
				/// a = (cP1 -P1)x(cP2-cP1)/((P2-P1)x(cP2-cP1))
				QVector3D C = currentPointTwo - currentPointOne;
				QVector3D P = pointOne - pointTwo;
				QVector3D offset = currentPointOne - pointOne;
				if(C == QVector3D(0,0,0) || P == QVector3D(0,0,0) || offset == QVector3D(0,0,0))
				{
					/// can't continue, either intersect at corner or have a zero length line
					continue;
				}
				QVector3D PcrossC = QVector3D::crossProduct(P,C);

				// COMMENTED OUT
				QVector3D offsetcrossC = QVector3D::crossProduct(offset,C);
				//

				if(PcrossC == QVector3D(0,0,0))
				{
					// can't find this point
					continue;
				}

				// COMMENTED OUT
				double a1 = offsetcrossC.x()/PcrossC.x();
				double a2 = offsetcrossC.y()/PcrossC.y();
				double a3 = offsetcrossC.z()/PcrossC.z();
				qdebug()<<"Edge"<<m<<"Compared with edge"<<k;
				qdebug()<<"Results are "<<a1<<a2<<a3;
				//
			}
			*/
			alternateProjectedCoordinates<<QVector3D(newUsedBeam.at(pcount).x(),newUsedBeam.at(pcount).y(),zComponent);
			MatrixXd coordinate (1,3);
			coordinate = vectorToMatrix(alternateProjectedCoordinates.at(pcount),coordinate);
			newUsedBeamCoords<<matrixToVector(coordinate*planeMatrix.transpose());
		}

		zComponent = alternateIntersectionCoordinates.at(0).z();

		alternateIntersectionCoordinates.clear();

		for(int pcount = 0; pcount < newIntersectionShape.count(); pcount++)
		{
			QVector3D point = QVector3D(newIntersectionShape.at(pcount).x(),newIntersectionShape.at(pcount).y(),zComponent);
			alternateIntersectionCoordinates<<point;
			MatrixXd coordinate (1,3);
			coordinate = vectorToMatrix(point,coordinate);
			if(!debuggingSuppressed_)
				AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Coordinate: ") );
			debugPrintMatrix(coordinate);
			newIntersectionCoords<<matrixToVector(coordinate*planeMatrix.transpose());
//>>>>>>> Issue496ToMergeWithSGMRelease
		}


		usedBeam.clear();
		usedBeam = newUsedBeamCoords;
		intersectionShapes_[i.value()].clear();
		intersectionShapes_[i.value()] = newIntersectionCoords;




		i++;
	}


	/// update the intersections
	intersections_.clear();
	for(int i = 0; i < intersectionShapes_.count(); i++)
	{
		intersections_<<intersectionScreenShape(intersectionShapes_.at(i));
		if(!debuggingSuppressed_)
		{
			for(int x = 0, size = intersections_.count(); x < size; x++)
				AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Intersection (point %1) %2: %3, %4").arg(x).arg(i).arg(intersections_.at(i).at(x).x()).arg(intersections_.at(i).at(x).y()) );
			for(int x = 0, size = intersectionShapes_.at(i).count(); x < size; x++)
				AMErrorMon::debug(this, AMSAMPLECAMERA_DEBUG_OUTPUT, QString("Intersection %1 %2: %3 %4 %5").arg(i).arg(x).arg(intersectionShapes_.at(i).at(x).x()).arg(intersectionShapes_.at(i).at(x).y()).arg(intersectionShapes_.at(i).at(x).z()) );
		}
	}


	delete distances;
}

/// determines if two vectors are equal to the specified precision
bool AMSampleCamera::equalVectors(QVector3D a, QVector3D b, double precision) const
{
	double maxError = 0;
	double newError = 0;
	double aV [3];
	double bV [3];
	aV[0] = a.x();
	aV[1] = a.y();
	aV[2] = a.z();
	bV[0] = b.x();
	bV[1] = b.y();
	bV[2] = b.z();

	for(int i = 0; i < 3; i++)
	{
		/*
		if(aV[i] == 0 && bV[i] == 0)
		{
			maxError = maxError;
		}
		else if(bV[i] == 0)
		{
			newError = aV[i]<0?-1*aV[i]:aV[i];
			maxError = newError>maxError?newError:maxError;
		}
		else if(aV[i] == 0)
		{
			newError = bV[i]<0?-1*bV[i]:bV[i];
			maxError = newError>maxError?newError:maxError;
		}
		else
		{
			newError = 1-aV[i]/bV[i];
			newError = newError<0?-1*newError:newError;
			maxError = newError>maxError?newError:maxError;
		}
		*/

		if(aV[i] == 0 && bV[i] == 0)
			newError = maxError;
		else if(bV[i] == 0)
			newError = fabs(aV[i]);
		else if(aV[i] == 0)
			newError = fabs(bV[i]);
		else
			newError = fabs(1-aV[i]/bV[i]);
		if(newError > maxError)
			maxError = newError;
	}
	return maxError<precision;
}




