#include "AMSampleCamera.h"

#include <QGraphicsRectItem>
#include <QResizeEvent>
#include <QVector3D>

#include <QMediaObject>
#include <QGraphicsVideoItem>
#include <QDebug>
#include <math.h>
#include "beamline/camera/AMCameraConfiguration.h"
#include "beamline/camera/AMCamera.h"


#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
//#include "beamline/SGM/SGMMAXvMotor.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMSamplePre2013.h"
#include "dataman/AMSample.h"

#include <QVector4D>

#include "dataman/AMSamplePlate.h"
#include "source/beamline/AMBeamline.h"



#define TOPLEFT 0
#define TOPRIGHT 1
#define BOTTOMRIGHT 2
#define BOTTOMLEFT 3
#define TOPCLOSE 4
#define RECTANGLE_POINTS 5

using namespace Eigen;

AMSampleCamera* AMSampleCamera::set_;

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

/// returns motor x coordinate
double AMSampleCamera::motorX() const
{
	return motorCoordinate_.x();
}

/// returns motor y coordinate
double AMSampleCamera::motorY() const
{
	return motorCoordinate_.y();
}

/// returns motor z coordinate
double AMSampleCamera::motorZ() const
{
	return motorCoordinate_.z();
}

/// returns the motor rotation
double AMSampleCamera::motorRotation() const
{
	return motorRotation_;
}

/// returns the intersections
QVector<QPolygonF> AMSampleCamera::intersections() const
{
	return intersections_;
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

/// returns the crosshair point
QPointF AMSampleCamera::crosshair() const
{
	return crosshair_;
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

QVector3D AMSampleCamera::rotationalOffset() const
{
	return rotationalOffset_;
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
	motorCoordinate_.setX(x);
	motorCoordinate_.setY(y);
	motorCoordinate_.setZ(z);

	motorRotation_ = r;
}

/// sets the motor coordinate to xyzr
void AMSampleCamera::setMotorCoordinate()
{
	double x = ssaManipulatorX_->value();
	double y = ssaManipulatorY_->value();
	double z = ssaManipulatorZ_->value();
	double r = ssaManipulatorRot_->value();
	motorMovement(x,y,z,r);
	motorCoordinate_.setX(x);
	motorCoordinate_.setY(y);
	motorCoordinate_.setZ(z);



	motorRotation_ = r;

        // update the sample plate
        AMSamplePlate* currentPlate = AMBeamline::bl()->samplePlate();
        if(currentPlate)
        {
            currentPlate->setPlatePosition(motorCoordinate_);
            currentPlate->setPlateRotation(motorRotation_);
        }
}


/// toggles distortion on or off
void AMSampleCamera::toggleDistortion()
{

	bool newValue = !distortion_;
	distortion_ = newValue;
        if(distortion_)
        {
                qDebug()<<"Distortion on";
        }
        if(!distortion_)
        {
                qDebug()<<"Distortion off";
        }
	updateAllShapes();
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

/// set the crosshair point
void AMSampleCamera::setCrosshair(QPointF crosshair)
{
	crosshair_ = crosshair;
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
	{
		qDebug()<<"AMSampleCamera::setOtherDataTwo - setting data to"<<data;
		shapeList_[index]->setOtherDataFieldTwo(data);
	}
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

void AMSampleCamera::setRotationalOffset(const QVector3D &offset)
{
	rotationalOffset_ = offset;
	centerOfRotation_ = motorCoordinate_ + rotationalOffset_;
	emit rotationalOffsetChanged(rotationalOffset_);
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
	QVector3D currentPosition = motorCoordinate_;
	QVector3D shiftAmount = currentPosition - platePosition;
	oldRotation_ = plate->plateRotation()/180*M_PI;
        qDebug()<<"AMSampleCamera::onSamplePlateLoaded - old position"<<platePosition;
        qDebug()<<"AMSampleCamera::onSamplePlateLoaded - cur position"<<currentPosition;
	qDebug()<<"AMSampleCamera::onSamplePlateLoaded"<<oldRotation_;
	double rotation = motorRotation()/180*M_PI;
	foreach(AMShapeData* shape, shapeList_)
	{
		shape->shift(shiftAmount);
		shape->setCoordinateShape(applyMotorRotation(shape, rotation));
	}
	oldRotation_ = rotation;
	emit motorCoordinateChanged(motorCoordinate_);
	emit motorRotationChanged(motorRotation_);
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
	}
	/// auto draw shapes at the points, for convenience
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

}

/// finds all the intersections, returns false if there is no valid beam
bool AMSampleCamera::findIntersections()
{
	if(beamModel_->positionOne().isEmpty() || beamModel_->positionTwo().isEmpty())
	{
		return false;
	}
	intersections_.clear();
	for(int i = 0; i <= index_; i++)
	{
		QVector<QVector3D> intersectionShape = findIntersectionShape(i);
		if(!intersectionShape.isEmpty())
			intersections_<<intersectionScreenShape(intersectionShape);
	}

	if(samplePlateSelected_)
	{
		QVector<QVector3D> intersectionShape = findIntersectionShape(samplePlateShape_, false);
		if(!intersectionShape.isEmpty())
			intersections_<<intersectionScreenShape(intersectionShape);
	}
		return true;
}

void AMSampleCamera::deleteShape(int index)
{
	if(isValid(index))
	{
                AMShapeData* shape = takeItem(index);
                shape->deleteLater();
	}
}

void AMSampleCamera::removeShapeData(AMShapeData *shape)
{
    AMShapeData *shapeToDelete;
    if(!shape)
    {
        qDebug()<<"AMSampleCamera::removeShapeData - Shape is null, operation failed";
        return;
    }
    if(shapeList_.contains(shape))
    {
        shapeToDelete = takeItem(shapeList_.indexOf(shape));
        if(shape != shapeToDelete)
        {
            qDebug()<<"AMSampleCamera::removeShapeData - failed to remove shape";
            insertItem(shapeToDelete);
        }
        else
        {
            shape->deleteLater();
            shape = 0;
        }
    }
}

int AMSampleCamera::rowCount(const QModelIndex &parent) const
{
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

	JacobiSVD<MatrixXd> svd(mMatrix,ComputeThinV|ComputeThinU);
	svd.compute(mMatrix,ComputeThinU|ComputeThinV);
	// solution is first column of V matrix - solution for maximum singular value of m
	MatrixXd vMatrix = svd.matrixV();
	MatrixXd solution = vMatrix.col(0);
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


/// creates a new rectangle, and initializes its data
void AMSampleCamera::startRectangle(QPointF position)
{

	position = undistortPoint(position);
	QVector<QVector3D> newShape;
	QVector3D coordinate[RECTANGLE_POINTS];
	double depth = camera_->focalLength();
	/// if drawing on shape, the point to start at lies
	/// on the shape to draw on.
	if(drawOnShapeEnabled_ && drawOnShapeSelected_)
	{
		QVector3D heightNormal = getHeightNormal(drawOnShape_);
		QVector3D widthNormal = getWidthNormal(drawOnShape_);
		QVector3D normal = getNormal(heightNormal, widthNormal);
		QVector3D rightNormal = getNormal(normal,heightNormal);
		QVector3D startPoint = getPointOnShape(position, normal);
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
//    updateShape(index_);

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
	if(drawOnShapeEnabled_ && drawOnShapeSelected_)
	{
		bottomRight = getPointOnShape(position,getNormal(drawOnShape_));
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
	if(topRight.isNull() && bottomLeft.isNull()) qDebug()<<"null shape";
	else if(topRight.isNull()) topRight = QVector3D::normal(QVector3D::normal(bottomLeft,shift),bottomLeft);
	else if(bottomLeft.isNull()) bottomLeft = QVector3D::normal(QVector3D::normal(topRight,shift),topRight);


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
					delete calibrationPoints_[j];
					calibrationPoints_[j] = 0;
					j = SAMPLEPOINTS;
					deleted = true;

				}
				if(j < 3)
				{
					if(polygon == beamMarkers_[j])
					{
						delete beamMarkers_[j];
						beamMarkers_[j] = 0;
						j = SAMPLEPOINTS;
						deleted = true;
					}
				}
			}
			if(!deleted && (polygon == samplePlateShape_))
			{
				delete samplePlateShape_;
				samplePlateShape_ = 0;
				deleted = true;
			}
			if(!deleted && (polygon == cameraConfigurationShape_))
			{
				delete cameraConfigurationShape_;
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

		currentVector_ = undistortPoint(position);
	}
	else
	{
		setCurrentIndex(index_ + 1);
	}
}

/// moves the currently selected rectangle by position + currentVector_
void AMSampleCamera::moveCurrentShape(QPointF position, int index)
{
	/// \todo store oldPosition as a vector?
	position = undistortPoint(position);
	if(index == -1) index = currentIndex_;
	if(index <= index_ && isValid(index))
	{
		QVector3D newPosition;
		QVector3D oldPosition;
		QVector3D shift;
		if(moveOnShape() && samplePlateSelected_)
		{
			newPosition = getPointOnShape(samplePlateShape_,position);
			oldPosition = getPointOnShape(samplePlateShape_,currentVector_);
			shift = newPosition - oldPosition;
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
	currentVector_ = (position);
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

/// moves the clicked point to appear under the crosshair, and gives the predicted motor coordinate
void AMSampleCamera::shiftToPoint(QPointF position, QPointF crosshairPosition)
{
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
	else return;
		QVector3D oldCoordinate = shape->coordinate(TOPLEFT);
		QVector3D currentPosition = getPointOnShape(shape, position);
		QVector3D newPosition = camera_->transform2Dto3D(crosshairPosition, depth(oldCoordinate));
		if(moveToBeam())
			newPosition = beamIntersectionPoint(currentPosition);
//		newPosition = currentPosition;
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
			inboardOutboard = motorCoordinate_.x();
			upStreamDownStream = motorCoordinate_.y();
			upDown = motorCoordinate_.z();

		}
		else
		{
			shift += motorCoordinate_;
			inboardOutboard = shift.x();
			upStreamDownStream = shift.y();
			upDown = shift.z();

			moveMotors(inboardOutboard, upStreamDownStream, upDown);

		}
		emit motorCoordinateChanged(motorCoordinate_);
		emit motorRotationChanged(motorRotation_);


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

void AMSampleCamera::drawShape(QPointF position)
{
	currentPolygon_<<position;
}

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
		if(drawOnShapeEnabled_ && drawOnShapeSelected_)
		{
			 coordinates<<getPointOnShape(currentPolygon_.at(i),getNormal(drawOnShape_));
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
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }

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
    else
    {
        qDebug()<<"Invalid index, cannot add beam shape";
    }

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
	for(int i = 0; i < 3; i++)
	{
		int index = shapeList_.indexOf(beamMarkers_[i]);
		if(index >= 0)
		{
			AMShapeData* polygon = takeItem(index);
			if(polygon == beamMarkers_[i])
			{
                                beamMarkers_[i]->deleteLater();
				beamMarkers_[i] = 0;
				polygon = 0;
                                // deleted the actual markers, now need to remove them from the sample plate.
			}
			else
			{
				qDebug()<<"AMSampleCamera::beamCalibrate - error in deleting shapes";
				insertItem(polygon);
			}
		}
	}

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
				qDebug()<<"AMSampleCamera::setSamplePlate - cannot set sample plate to be a beam shape.";
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
                samplePlateShape_->copy(currentShape());//     takeItem(currentIndex_);


                AMSamplePlate *currentPlate = AMBeamline::bl()->samplePlate();
                AMSample *currentSample = currentPlate->sampleFromShape(currentShape(currentIndex_));
                AMShapeData* currentShape = takeItem(currentIndex_);
                currentShape->deleteLater();
                if(currentSample)
                {
                    currentPlate->removeSample(currentSample);
                }

		samplePlateShape_->setName("Sample Plate");
                qDebug()<<"AMSampleCamera::setSamplePlate - getting name";
                qDebug()<<"AMSampleCamera::setSamplePlate - sample plate name is "<<samplePlateShape_->name();
		samplePlateShape_->setOtherDataFieldOne("Sample Plate");
                samplePlateSelected_ = true;
                setDrawOnSamplePlate();



	}

}

void AMSampleCamera::setSamplePlate(AMShapeData *samplePlate)
{
//	insertItem(samplePlate);
    samplePlateShape_ = samplePlate;
    if(samplePlateShape_->name() != "Sample Plate")
    {
        samplePlateShape_->setName("Sample Plate");
    }
    samplePlateShape_->setOtherDataFieldOne("Sample Plate");
    samplePlateSelected_ = true;
    setDrawOnSamplePlate();
}

void AMSampleCamera::createSamplePlate(QVector<QVector3D> coordinates, QVector<QPointF> points)
{
    const int NUMBER_OF_COORDINATES = 2;
    const int MINIMUM_NUMBER_OF_POINTS = 4; // minimal shape is a triangle, 2 points for each coordinate.
    // one coordinate already known
	if(coordinates.count() != NUMBER_OF_COORDINATES)
    {
        qDebug()<<"AMSampleCamera::createSamplePlate - unexpected number of coordinates";
        qDebug()<<"have "<<coordinates.count()<<" expected "<<NUMBER_OF_COORDINATES;
    }
	if(points.count() < MINIMUM_NUMBER_OF_POINTS)
    {
        qDebug()<<"AMSampleCamera::createSamplePlate - unexpected number of points";
        qDebug()<<"have "<<points.count()<<" expected at least "<<MINIMUM_NUMBER_OF_POINTS;
    }
    QVector3D origin = coordinates.at(0);
    QVector3D shift = coordinates.at(1);
	// points should be in pairs for topright, bottomright,bottomleft, in that order
	QVector<QVector3D> plateCoordinates;
	for(int i = 0; i < points.count(); i = i + 2)
	{
		QPair<QPointF, QPointF> pointPair;
		pointPair.first = points.at(i);
		pointPair.second = points.at(i + 1);
		plateCoordinates<<findSamplePlateCoordinate(origin, shift, pointPair);
	}

}

QVector3D AMSampleCamera::findSamplePlateCoordinate(QVector3D originCoordinate, QVector3D shiftCoordinate, QPair<QPointF, QPointF> points)
{
    /// This code finds the two coordinates corresponding to the two point and
    /// two coordinates
    qDebug()<<"AMSampleCamera::createSamplePlate";


	QPointF originPoint = points.first;
	QPointF shiftPoint = points.second;


	QVector3D topRightOriginBase = camera_->transform2Dto3D(originPoint,1);
	QVector3D topRightOriginLength = camera_->transform2Dto3D(originPoint, 2);
    QVector3D topRightOriginVector = topRightOriginLength-topRightOriginBase;
    topRightOriginVector.normalize();
    QVector3D originBase = topRightOriginBase - topRightOriginVector;
    // originBase + tOrigin*topRightOriginVector gives 3D position of topRight

	QVector3D topRightShiftBase = camera_->transform2Dto3D(shiftPoint,1);
	QVector3D topRightShiftLength = camera_->transform2Dto3D(shiftPoint, 2);
    QVector3D topRightShiftVector = topRightShiftLength-topRightShiftBase;
    topRightOriginVector.normalize();
    QVector3D shiftBase = topRightShiftBase - topRightShiftVector;
    // shiftBase + tShift*topRightShiftVector gives 3D position of topRight shifted
    // shift base should be equal to origin base

    // overdetermined system?

    //t1 = (zoxsy-sxzoy)/(zixzoy-ziyzox)
    //t0 = (sx + tizix)/zox
    double tShift; // this is the length of the shifted line
    double tOrigin; // this is the length of the origin line
	QVector3D shift = shiftCoordinate - originCoordinate;
    tShift = (originBase.x()*shift.y()-shift.x()*originBase.y())/(shiftBase.x()*originBase.y()-shiftBase.y()*originBase.x());
    tOrigin = (shift.x() + tShift*shiftBase.x())/originBase.x();

    // putting this into the third equation should yield similar results...
    // if it doesn't agree, may need to do DLT on the system
    double tOriginTest = (shift.z() + tShift*shiftBase.z())/originBase.z();

    qDebug()<<"tShift"<<tShift;
    qDebug()<<"tOrigin"<<tOrigin;
    qDebug()<<"tOriginTest"<<tOriginTest;
	return originBase + tOrigin*topRightOriginVector; // this is the "origin" "top right" coordinate.
}

void AMSampleCamera::saveSamplePlate()
{
    if(samplePlateSelected_ && samplePlateShape_)
    {
        AMSample* samplePlate = new AMSample();
        samplePlate->setName(samplePlateShape_->name());
        samplePlate->setSampleShapePositionData(samplePlateShape_);
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

void AMSampleCamera::addBeamMarker(int index)
{
	if(beamMarkers_[index])
	{
		qDebug()<<"Beam marker is null";
		deleteShape(shapeList_.indexOf(beamMarkers_[index]));
	}
	qDebug()<<"creating new shape";
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

void AMSampleCamera::moveMotorTo(QVector3D coordinate)
{
	double x = coordinate.x();
	double y = coordinate.y();
	double z = coordinate.z();
	if(enableMotorMovement_)
	{
		bool success = moveMotors(x,y,z);
		if(!success)
			qDebug()<<"AMSampleCamera::moveMotorTo - failed to moveMotors";
	}
	else
	{
		qDebug()<<"Cannot move motors.";
	}

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
	else
	{
		qDebug()<<"AMSampleCamera::stopMotors - cannot stop motors, movement is not enabled";
	}
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
               if(shapeRetrieved != sample->sampleShapePositionData())
               {
                   qDebug()<<"AMSampleCamera::removeSample failed";
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
        qDebug()<<"AMSampleCamera::loadDefaultSamplePlate - Sample Plate original position is"<<samplePlate->centerCoordinate();
//        samplePlate->shift(motorCoordinate_);
        qDebug()<<"AMSampleCamera::loadDefaultSamplePlate - Sample Plate position is"<<samplePlate->centerCoordinate();
	setSamplePlate(samplePlate);
	saveSamplePlate();
}


/// tracks motor movement and shifts drawings accordingly
void AMSampleCamera::motorTracking(double)
{
	/// update coordinates, line edits, don't move the motor
	double tolerance = 0.01;
	double motorX = ssaManipulatorX_->value();
	double motorZ = ssaManipulatorZ_->value();
	double motorY = ssaManipulatorY_->value();
	double motorRotation = ssaManipulatorRot_->value();
	if(fabs(motorX-motorCoordinate_.x()) < tolerance && fabs(motorY-motorCoordinate_.y()) < tolerance && fabs(motorZ-motorCoordinate_.z()) < tolerance && fabs(motorRotation-motorRotation_) < tolerance)
	{
		return;
	}


	emit motorMoved();

//	setMotorCoordinate(motorX,motorY,motorZ,motorRotation);
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
	AMDatabase* db = AMDatabase::database("user");
	QList<int> matchList = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(),"name", beamModel_->name());
	if(matchList.count() != 0)
	{
		beamModel_->dissociateFromDb();
		QVariantList beamList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(), "name");
		int beamCount = beamList.count();
		beamModel_->setName("beam"+QString::number(beamCount));
	}
	beamModel_->storeToDb(db);
}

void AMSampleCamera::saveCamera()
{
	AMCameraConfiguration* cameraConfiguration = camera_->cameraConfiguration();
	AMDatabase* db = AMDatabase::database("user");
//	QList<int> matchList = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMCameraConfiguration>(), "name", cameraConfiguration->name());
	cameraConfiguration->storeToDb(db);
}

/// Constructor
AMSampleCamera::AMSampleCamera(QObject *parent) :
	QAbstractListModel(parent)
{
	crosshair_ = QPointF(0.5,0.5);
	crosshairLocked_ = false;
	index_ = -1;
	camera_ = new AMCamera();
	beamModel_ = new AMBeamConfiguration();
	centerOfRotation_ = QVector3D(0,0,0);
	directionOfRotation_ = QVector3D(0,-1,0);

	enableMotorMovement_ = false;
	drawOnShapeEnabled_ = false;
	drawOnShapeSelected_ = false;
	samplePlateSelected_ = false;

	distortion_ = true;

	overrideMouseSelection_ = false;
	moveToBeam_ = true;
	moveOnShape_ = true;

	rotationalOffset_ = QVector3D(10.5,2.35,0);

	emit rotationalOffsetChanged(rotationalOffset_);

	oldRotation_ = 0;


	for(int i= 0; i < SAMPLEPOINTS; i++)
	{
		calibrationPoints_[i] = new AMShapeData();
		if(i < 3)
		{
			beamMarkers_[i] = new AMShapeData();
		}
	}


	AMDatabase *db = AMDatabase::database("user");

	if(!db){
		// create the database
		db = AMDatabase::createDatabase("user", "/home/sgm/AcquamanData/userdata.db");
		if(!db)
			qDebug() << "Uh oh, no database created";
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

			qDebug() << "Status of registration is " << success;
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
		if(!success)qDebug()<<"Failed to store default beam to database, in AMSampleCamera constructor";
	}
	else
	{
		beamModel_->loadFromDb(db,matchIDs.first());
	}

	/// add motor manipulators

	ssaManipulatorX_ = new CLSMAXvMotor("ssaManipulatorX", "SMTR16114I1022", "SSA Inboard/Outboard", true, 0.2, 2.0, this);
	ssaManipulatorX_->setContextKnownDescription("X");

	ssaManipulatorY_ = new CLSMAXvMotor("ssaManipulatorY", "SMTR16114I1023", "SSA Upstream/Downstream", true, 0.2, 2.0, this);
	ssaManipulatorY_->setContextKnownDescription("Y");

	ssaManipulatorZ_ = new CLSMAXvMotor("ssaManipulatorZ", "SMTR16114I1024", "SSA Up/Down", true, 0.2, 2.0, this);
	ssaManipulatorZ_->setContextKnownDescription("Z");

	ssaManipulatorRot_ = new CLSMAXvMotor("ssaManipulatorRot", "SMTR16114I1025", "SSA Rotation", false, 0.2, 2.0, this);
	ssaManipulatorRot_->setContextKnownDescription("R");

	//ssaManipulatorX_ = new SGMMAXvMotor("ssaManipulatorX", "SMTR16114I1022", "SSA Inboard/Outboard", true, 0.2, 2.0, this);
	//ssaManipulatorX_->setContextKnownDescription("X");

	//ssaManipulatorY_ = new SGMMAXvMotor("ssaManipulatorY", "SMTR16114I1023", "SSA Upstream/Downstream", true, 0.2, 2.0, this);
	//ssaManipulatorY_->setContextKnownDescription("Y");

	//ssaManipulatorZ_ = new SGMMAXvMotor("ssaManipulatorZ", "SMTR16114I1024", "SSA Up/Down", true, 0.2, 2.0, this);
	//ssaManipulatorZ_->setContextKnownDescription("Z");

	//ssaManipulatorRot_ = new SGMMAXvMotor("ssaManipulatorRot", "SMTR16114I1025", "SSA Rotation", false, 0.2, 2.0, this);
	//ssaManipulatorRot_->setContextKnownDescription("R");


	connect(&motorUpdateDeferredFunction_, SIGNAL(executed()), this, SLOT(setMotorCoordinate()));//(double,double,double,double)));


	connect(camera_, SIGNAL(configurationChanged(AMCameraConfiguration*)),this, SIGNAL(cameraConfigurationChanged(AMCameraConfiguration*)));

	connect(ssaManipulatorX_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
	connect(ssaManipulatorY_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
	connect(ssaManipulatorZ_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));
	connect(ssaManipulatorRot_, SIGNAL(moveSucceeded()), this, SLOT(motorsFinishedMoving()));


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
	return applySpecifiedRotation(shape, directionOfRotation_, centerOfRotation_, rotation - oldRotation_)->coordinates();
}


AMShapeData* AMSampleCamera::applySpecifiedRotation(const AMShapeData *shape, QVector3D direction, QVector3D centre, double angle) const
{
	AMShapeData* newShape = new AMShapeData();
	newShape->copy(shape);
	for(int i = 0; i < newShape->count(); i++)
	{
		newShape->setCoordinate(rotateCoordinate(newShape->coordinate(i),centre,direction,angle),i);
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
		qDebug()<<"Unknown axis - defaulting to z axis";
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
QVector3D AMSampleCamera::rotateCoordinate(QVector3D coordinate, QVector3D center, QVector3D direction, double rotation) const
{
	double u = direction.x();
	double v = direction.y();
	double w = direction.z();

	double x = coordinate.x();
	double y = coordinate.y();
	double z = coordinate.z();


	/// Each new coordinate has three terms - a 1-cos, a cos and a sin term
	/// to simplify the math, they will be computed as 3D vectors
	/// the xyz components will be the dot product of the terms with the trig terms

	QVector3D trigTerms(1-cos(rotation),cos(rotation),sin(rotation));

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

/// applies rotation, tilt, and distortion to the shape
QPolygonF AMSampleCamera::subShape(const AMShapeData* shape) const
{
    if(!(shape))
    {
        qDebug()<<"AMSampleCamera::subShape - shape is null";
    }
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

//	if(motorRotation_ != 0)
//	{
//		rotatedShape = applyMotorRotation(rotatedShape,(motorRotation()/180*M_PI));
//	}

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

//	QVector3D centerPlatePlane = samplePlateShape_->centerCoordinate();
//	rotationalOffset += centerPlatePlane;
	qDebug()<<"AMSampleCamera::motorMovement"<<x<<y<<z<<r;
	QVector3D newPosition(x,y,z);
	QVector3D shift = newPosition - motorCoordinate_;
	QVector3D centerOfRotation = newPosition + rotationalOffset_;
	QVector3D directionOfRotation = QVector3D(0,0,1);
	centerOfRotation_ = centerOfRotation;
	directionOfRotation_ = directionOfRotation;
	double rotation = r/180*M_PI;



	for(int i = 0; i <= index_; i++)
	{
		if(shapeList_[i] != beamMarkers_[0] && shapeList_[i] != beamMarkers_[1] && shapeList_[i] != beamMarkers_[2])
		{
			shiftCoordinates(shift,i);
			applyMotorRotation(i, rotation);
			updateShape(i);
		}
	}
	if(samplePlateSelected_)
	{
		shiftCoordinates(shift,samplePlateShape_);
		samplePlateShape_->setCoordinateShape(applyMotorRotation(samplePlateShape_, rotation));
		updateShape(samplePlateShape_);
	}

	oldRotation_ = rotation;


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
QVector<QVector3D> AMSampleCamera::findIntersectionShape(int index) const
{
	return findIntersectionShape(shapeList_[index]);
}

QVector<QVector3D> AMSampleCamera::findIntersectionShape(const AMShapeData* shapeToIntersect, bool boundIntersection) const
{
	/// First find the shape on the same plane
	QVector<QVector3D> rotatedShape = rotateShape(shapeToIntersect);
	QVector<QVector3D> shape;
	QVector3D topLeft = rotatedShape.at(TOPLEFT);
	QVector3D hHat = getHeightNormal(shapeToIntersect);
	QVector3D wHat = getWidthNormal(shapeToIntersect);
	QVector3D nHat = QVector3D::normal(wHat,hHat);
	hHat = QVector3D::normal(nHat,wHat);
	int count = beamModel_->count();
	QVector3D l0 [count];
	QVector3D lHat [count];
	double distance[count];
	double numerator;
	double denominator;
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
//	shape<<shape.first();

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


			beamShape<<QPointF(beamWComponent[i],beamHComponent[i]);
		}

		if(i<shapeCount)
		{
			shapeHComponent[i] = dot(hHat,rotatedShape.at(i));
			shapeWComponent[i] = dot(wHat,rotatedShape.at(i));
			shapeNComponent[i] = dot(nHat,rotatedShape.at(i));

			originalShape<<QPointF(shapeWComponent[i],shapeHComponent[i]);
		}
	}

	QPolygonF intersection;
	if(boundIntersection)
		intersection = originalShape.intersected(beamShape);
	else
		intersection = beamShape;
	if(intersection.isEmpty()) return QVector<QVector3D>();

	QVector3D point;
	QPointF oldPoint;
	QVector<QVector3D> intersectionShape;

	for(int i = 0; i < intersection.count(); i++)
	{
		oldPoint = intersection.at(i);
		point = oldPoint.x()*wHat + oldPoint.y()*hHat + shapeNComponent[i%shapeCount]*nHat;
		intersectionShape<<point;
	}

	return intersectionShape;
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

QVector3D AMSampleCamera::getPointOnShape(const QPointF &position, const QVector3D &nHat) const
{
	return getPointOnShape(drawOnShape_, position, nHat);
}

QVector3D AMSampleCamera::getPointOnShape(const AMShapeData *shape, const QPointF &position, const QVector3D &nHat) const
{
	/// figure out line
	/// pick a depth to use, doesn't really matter what it is
	double depth = camera_->focalLength();
	QVector3D l0 = camera_->transform2Dto3D(position,depth);
	QVector3D lVector = camera_->transform2Dto3D(position,2*depth) - l0;
	QVector<QVector3D> rotatedShape = rotateShape(shape);
	double numerator = dot((rotatedShape.at(TOPLEFT) - l0), nHat);
	double denominator = dot(lVector,nHat);
	double distance = 0;
	if(denominator == 0)
	{
		if(numerator == 0)
		{
			qDebug()<<"AMSampleCamera::getPointOnShape - Line is on plane";
		}
		else
		{
			qDebug()<<"AMSampleCamera::getPointOnShape - Line is parallel to plane";
		}
	}
	else
	{
		distance = numerator/denominator;
	}
	QVector3D pointOnShape = l0 + distance*lVector;
	return pointOnShape;
}

QVector3D AMSampleCamera::getPointOnShape(const AMShapeData* shape, const QPointF &position) const
{
	return getPointOnShape(shape,position,getNormal(shape));
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

//void AMSampleCamera::removeItem(int index)
//{
//	index_--;
//	beginRemoveRows(QModelIndex(),index,index);
//	shapeList_.removeAt(index);
//	endRemoveRows();
//	emit shapesChanged();
//}

AMShapeData *AMSampleCamera::takeItem(int index)
{
	index_--;
	beginRemoveRows(QModelIndex(),index,index);
	AMShapeData* oldShape = shapeList_.takeAt(index);
	endRemoveRows();
	emit shapesChanged();
	return oldShape;
}

QVector3D AMSampleCamera::beamIntersectionPoint(QVector3D samplePoint)
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
	for(int i = 0; i < beamConfiguration()->count()-1; i++)
	{
		// equation for line: x = x0 + at , etc.
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
			qDebug()<<"Ray does not move in y direction, cannot intersect it";
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


	}
//	insertItem(newShape);
	newShape->setName(QString("Shape %1").arg(index_));
	updateShape(newShape);
	// find the center of the point
	beamSpot /= beamConfiguration()->count()-1;
	return  beamSpot;
}

/// moves the x, y, z motors
bool AMSampleCamera::moveMotors(double x, double y, double z)
{
	qDebug()<<"AMSampleCamera::moveMotors"<<x<<y<<z;
	bool success = false;
	if(motorMovementEnabled())
	{
		success = true;
		AMControl::FailureExplanation failure [3];
		double xMin = ssaManipulatorX_->minimumValue();
		qDebug()<<xMin;
		double xMax = ssaManipulatorX_->maximumValue();
		qDebug()<<xMax;
		double yMin = ssaManipulatorY_->minimumValue();
		double yMax = ssaManipulatorY_->maximumValue();
		double zMin = ssaManipulatorZ_->minimumValue();
		double zMax = ssaManipulatorZ_->maximumValue();
		if(x > xMax) x = xMax;
		else if(x < xMin) x = xMin;
		if(y > yMax) y = yMax;
		else if(y < yMin) y = yMin;
		if(z > zMax) z = zMax;
		else if(z < zMin) z = zMin;
		failure[0] = ssaManipulatorX_->move(x);
		failure[1] = ssaManipulatorY_->move(y);
		failure[2] = ssaManipulatorZ_->move(z);

		for(int i = 0; i < 3; i++)
		{
			if(failure[i] != AMControl::NoFailure)
			{
				success = false;
				qDebug()<<AMControl::failureExplanation(failure[i]);
			}
		}
	}
        return success;
}



























