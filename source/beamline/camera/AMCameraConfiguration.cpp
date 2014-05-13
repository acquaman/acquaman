#include "AMCameraConfiguration.h"
#include "dataman/database/AMDbObject.h"

#include <QVector3D>

#include "util/AMErrorMonitor.h"

AMCameraConfiguration::AMCameraConfiguration(QObject *parent) :
	AMDbObject(parent)
{
	matrixCalculated_ = false;
	cameraRotation_ = 0;
	imageCentre_ = QPointF(0,0);
}

double AMCameraConfiguration::cameraPositionX()
{
	return cameraPosition_.x();
}

double AMCameraConfiguration::cameraPositionY()
{
	return cameraPosition_.y();
}

double AMCameraConfiguration::cameraPositionZ()
{
	return cameraPosition_.z();
}

QVector3D AMCameraConfiguration::cameraPosition()
{
	return cameraPosition_;
}

double AMCameraConfiguration::cameraCentreX()
{
	return cameraCentre_.x();
}

double AMCameraConfiguration::cameraCentreY()
{
	return cameraCentre_.y();
}

double AMCameraConfiguration::cameraCentreZ()
{
	return cameraCentre_.z();
}

QVector3D AMCameraConfiguration::cameraCentre()
{
	return cameraCentre_;
}

double AMCameraConfiguration::cameraFOV()
{
	return cameraFOV_;
}

double AMCameraConfiguration::cameraFocalLength()
{
	return cameraFocalLength_;
}

double AMCameraConfiguration::cameraDistortion()
{
	return cameraDistortion_;
}

double AMCameraConfiguration::cameraRotation()
{
	return cameraRotation_;
}

double AMCameraConfiguration::pixelAspectRatio()
{
	return pixelAspectRatio_;
}

QPointF AMCameraConfiguration::imageCentre()
{
	return imageCentre_;
}

QVector<QVector3D> AMCameraConfiguration::cameraMatrix()
{
	return cameraMatrix_;
}

/// creates the matrix from the QVector, if it hasn't already been created
MatrixXd AMCameraConfiguration::cameraMatrixToMatrix()
{
	MatrixXd subMatrix[4];
	if(!matrixCalculated_ && !cameraMatrix_.isEmpty())
	{
		for(int i = 0; i < 4; i++)
		{
			subMatrix[i] = MatrixXd(3,1);
			subMatrix[i]<<cameraMatrix_.at(i).x(),cameraMatrix_.at(i).y(),cameraMatrix_.at(i).z();
		}
		MatrixXd cameraMatrix(3,4);
		cameraMatrix<<subMatrix[0],subMatrix[1],subMatrix[2],subMatrix[3];
		cameraTransformMatrix_ = cameraMatrix;
		matrixCalculated_ = true;
		return cameraMatrix;
	}
	else
	{
		return cameraTransformMatrix_;
	}
}

bool AMCameraConfiguration::hasMatrix()
{
	return !cameraMatrix_.isEmpty();
}

#include <QDebug>
bool AMCameraConfiguration::loadFromDb(AMDatabase *db, int id)
{
	return AMDbObject::loadFromDb(db, id);
}

void AMCameraConfiguration::setCameraPosition(QVector3D position)
{
	cameraPosition_ = position;
}

void AMCameraConfiguration::setCameraPositionX(double x)
{
	cameraPosition_.setX(x);
}

void AMCameraConfiguration::setCameraPositionY(double y)
{
	cameraPosition_.setY(y);
}

void AMCameraConfiguration::setCameraPositionZ(double z)
{
	cameraPosition_.setZ(z);
}

void AMCameraConfiguration::setCameraCentre(QVector3D centre)
{
	cameraCentre_ = centre;
}

void AMCameraConfiguration::setCameraCentreX(double x)
{
	cameraCentre_.setX(x);
}

void AMCameraConfiguration::setCameraCentreY(double y)
{
	cameraCentre_.setY(y);
}

void AMCameraConfiguration::setCameraCentreZ(double z)
{
	cameraCentre_.setZ(z);
}

void AMCameraConfiguration::setCameraFOV(double fov)
{
	cameraFOV_ = fov;
}

void AMCameraConfiguration::setCameraFocalLength(double focalLength)
{
	cameraFocalLength_ = focalLength;
}

void AMCameraConfiguration::setCameraDistortion(double distortion)
{
	cameraDistortion_ = distortion;
}

void AMCameraConfiguration::setCameraRotation(double rotation)
{
	cameraRotation_ = rotation;
}

void AMCameraConfiguration::setPixelAspectRatio(double pixelAspectRatio)
{
	pixelAspectRatio_ = pixelAspectRatio;
}

void AMCameraConfiguration::setImageCentre(QPointF imageCentre)
{
	imageCentre_ = imageCentre;
}

void AMCameraConfiguration::setImageCentreX(double imageCentreX)
{
	imageCentre_.setX(imageCentreX);
}

void AMCameraConfiguration::setImageCentreY(double imageCentreY)
{
	imageCentre_.setY(imageCentreY);
}

void AMCameraConfiguration::setCameraMatrix(QVector<QVector3D> cameraMatrix)
{
	cameraMatrix_ = cameraMatrix;
	matrixCalculated_ = false;
}

void AMCameraConfiguration::setCameraMatrixFromMatrix(MatrixXd cameraMatrix)
{
	if(cameraMatrix.rows() != 3)
		AMErrorMon::alert(this, AMCAMERACONFIGURATION_SETCAMERAMATRIXFROMMATRIX_INVALID_ROWS, QString("A call was made to AMCameraConfiguration::setCameraMatrixFromMatrix with the wrong number of rows. Expected: 3, Detected: %1.").arg(cameraMatrix.rows()) );
	if(cameraMatrix.cols() > 4)
		AMErrorMon::alert(this, AMCAMERACONFIGURATION_SETCAMERAMATRIXFROMMATRIX_INVALID_COLUMNS, QString("A call was made to AMCameraConfiguration::setCameraMatrixFromMatrix with the wrong number of columns. Expected: 4, Detected: %1.").arg(cameraMatrix.cols()) );
	QVector<QVector3D> newCameraMatrix;
	for(int i = 0; i < cameraMatrix.cols(); i++)
	{
		newCameraMatrix<<QVector3D(cameraMatrix(0,i),cameraMatrix(1,i),cameraMatrix(2,i));
	}
	int k = 4-cameraMatrix.cols();
	for(int j = 0; j < k; j++)
	{
		newCameraMatrix<<QVector3D(0,0,0);
		AMErrorMon::alert(this, AMCAMERACONFIGURATION_SETCAMERAMATRIXFROMMATRIX_LIKELY_INVALID_MATRIX, QString("A call was made to AMCameraConfiguration::setCameraMatrixFromMatrix and an invalid camera matrix was likely detected as it finished with zeros.") );
	}
	cameraMatrix_ = newCameraMatrix;
	cameraTransformMatrix_  = cameraMatrix;
	matrixCalculated_ = true;
}

