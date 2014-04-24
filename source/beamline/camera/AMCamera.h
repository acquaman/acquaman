#ifndef AMCAMERA_H
#define AMCAMERA_H

#include <QObject>
/// \todo use MPL Eigen only
/// #define EIGEN_MPL2_ONLY
#include <Eigen/Eigen>
#include <Eigen/SVD>
#include <Eigen/LU>
#include <Eigen/Dense>
#include <QSizeF>
#include <QVector3D>

class AMCameraConfiguration;
class QPointF;
class QVector3D;

using namespace Eigen;

#define AMCAMERA_DEBUG_OUTPUT 611001
#define AMCAMERA_DETERMINANT_NOT_CLOSE_TO_ONE 611002
#define AMCAMERA_TRANSFORM2DTO3D_NULL_VHAT 611003
#define AMCAMERA_TRANSFORM3DTO2D_NULL_VHAT 611004

/// AMCamera performs a camera transformation, both forward and in reverse.
/// That is, given a cameraConfiguration, it calculates where a 3D object appears
/// or where a 2D image resides in 3D space
class AMCamera : public QObject
{
	Q_OBJECT
public:
	AMCamera(QObject* parent = 0);

	AMCameraConfiguration* cameraConfiguration() const;
	void setCameraConfiguration(AMCameraConfiguration* cameraConfiguration);


	/// calculates the camera matrix
	void getTransforms(QPointF points[6],QVector3D coordinates [6]);

	/// transforms the given point and depth to a 3D coordinate
	/// based on the current camera model
	QVector3D transform2Dto3D(QPointF point, double depth) const;

	/// transforms a 3D vector to a 2D point, using current camera configuration
	QPointF transform3Dto2D(QVector3D coordinate) const;

	/// returns true if using matrix - can only be used if full calibration was performed
	bool useCameraMatrix() const;
	/// returns true if full calibration was performed
	bool calibrationRun() const;

	void setUseCameraMatrix(bool useCameraMatrix);
	void setCalibrationRun(bool calibrationRun);

	QSizeF viewSize() const;
	QSizeF scaledSize() const;

	void setViewSize(QSizeF viewSize);
	void setScaledSize(QSizeF scaledSize);

	double focalLength() const;

	/// distort a point
	QPointF distortPoint(QPointF point) const;

	/// remove distortion from a point
	QPointF undistortPoint(QPointF point) const;

	// general functions
	/// SVD solutions - non-homogenous and homogenous
	MatrixXd computeSVDLeastSquares(MatrixXd leftHandSide, MatrixXd rightHandSide) const;
	MatrixXd computeSVDHomogenous(MatrixXd leftHandSide) const;

signals:
	void configurationChanged(AMCameraConfiguration*);

protected:
	// general function
	double dot(QVector3D a, QVector3D b) const;

	/// transforms 2D point to 3D vector, using current camera matrix
	QVector3D transform2Dto3DMatrix(QPointF point, double depth) const;

	/// transforms 3D vector to 2D point, using current camera matrix
	QPointF transform3Dto2DMatrix(QVector3D coordinate) const;

	/// solves for the DLT of the homogenous equation relating coordinates to points
	MatrixXd directLinearTransform(QVector3D coordinate[6], QPointF screenPosition[6]) const;

	/// constructs a matrix to use in the DLT
	MatrixXd constructMatrix(QVector3D coordinate[6], QPointF screenposition[6]) const;

	/// solves for the intrinsic camera parameters, given part of the total camera matrix
	MatrixXd intrinsicParameters(MatrixXd matrixB) const;

	/// solves for the rotation matrix, given the intrinsic matrix and part of the total camera matrix
	MatrixXd rotationParameters(MatrixXd matrixA, MatrixXd matrixB) const;

	/// solves for the translation matrix, given the intrinsic matrix and part of the total camera matrix
	MatrixXd translationParameters(MatrixXd matrixA, MatrixXd matrixSubB) const;

	/// finds absolute coordinates from a coordinate and an extrinsic Matrix
	MatrixXd findWorldCoordinate(MatrixXd matrix, MatrixXd extrinsicMatrix) const;

	// general functions

	/// checks to see if two numbers are not equal, within the given tolerance
	bool notEqual(double a, double b, double tolerance = 0.001) const;

	/// returns zero if a number is near zero, within the given tolerance
	double nearZero(double a, double tolerance = 0.00001) const;

	/// finds the absolute error beteween two numbers
	double absError(double a, double b, double tolerance = 0.00001) const;

	/// returns a,b, and c for the equation of a line ( x = x0 + at, y = y0 + bt, z = z0 + ct)
	/// given two points and the distance between them (t).
	/// returns the values as a vector
	QVector3D lineParameters(QVector3D pointOne, QVector3D pointTwo, double t) const;
	//

protected:
	AMCameraConfiguration* cameraConfiguration_;
	bool useCameraMatrix_;
	bool calibrationRun_;

	QSizeF viewSize_;
	QSizeF scaledSize_;
};

#endif // AMCAMERA_H
