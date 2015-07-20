#ifndef BIOXASMIRRORMOTOR_H
#define BIOXASMIRRORMOTOR_H

#include "beamline/CLS/CLSMAXvMotor.h"

class BioXASMirrorMotor : public CLSMAXvMotor
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASMirrorMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double xPosition, double yPosition, double tolerance = 0.05, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0, const QString &pvUnitFieldName = ":mm");
	/// Destructor.
	virtual ~BioXASMirrorMotor();

	/// Returns the x position constant. +x is beam direction.
	double xPosition() const { return xPosition_; }
	/// Returns the y position constant. +y is outboard.
	double yPosition() const { return yPosition_; }
	/// Returns the z position. +z is up.
	double zPosition() const { return value(); }

signals:
	/// Notifier that the x position constant has changed.
	void xPositionChanged(double newPosition);
	/// Notifier that the y position constant has changed.
	void yPositionChanged(double newPosition);
	/// Notifier that the z position constant has changed.
	void zPositionChanged(double newPosition);

public slots:
	/// Sets the x position constant.
	AMControl::FailureExplanation setXPosition(double newPosition);
	/// Sets the y position constant.
	AMControl::FailureExplanation setYPosition(double newPosition);
	/// Sets the z position, moving the control.
	AMControl::FailureExplanation setZPosition(double newPosition) { return move(newPosition); }

protected:
	/// The x position.
	double xPosition_;
	/// The y position.
	double yPosition_;
};

#endif // BIOXASMIRRORMOTOR_H
