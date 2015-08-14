#ifndef CLSSIS3820SCALERMODECONTROL_H
#define CLSSIS3820SCALERMODECONTROL_H

#include "beamline/AMPseudoMotorControl.h"

class CLSSIS3820ScalerModeControl : public AMPseudoMotorControl
{
    Q_OBJECT

public:
	/// Enum describing the possible mode states.
	class Mode { public: enum State { SingleShot = 0, Continuous = 1, None = 2 }; };
	/// Enum describing the possible scan states.
	class Scan { public: enum State { NotScanning = 0, Scanning = 1 }; };
	/// Constructor.
	explicit CLSSIS3820ScalerModeControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = "");
	/// Destructor.
	virtual ~CLSSIS3820ScalerModeControl();

	/// Returns true if the control value is always measureable, provided it is connected. False otherwise.
	virtual bool shouldMeasure() const { return true; }
	/// Returns true if a control move is always possible, provided it is connected. False otherwise.
	virtual bool shouldMove() const { return true; }
	/// Returns true if a control stop is always possible, provided it is connected. False otherwise.
	virtual bool shouldStop() const { return false; }

	/// Returns true if this control's value can be measured right now. False otherwise.
	virtual bool canMeasure() const { return true; }
	/// Returns true if this control can move right now. False otherwise.
	virtual bool canMove() const;
	/// Returns true if this control can stop right now. False otherwise.
	virtual bool canStop() const { return false; }

	/// Returns the scan count control.
	AMControl* scanCountControl() const { return scanCountControl_; }
	/// Returns the number of scans per buffer control.
	AMControl* numberOfScansPerBufferControl() const { return numberOfScansPerBufferControl_; }
	/// Returns the start scan control.
	AMControl* startScanControl() const { return startScanControl_; }

	/// Returns true if the given value is a valid value for this control. False otherwise.
	virtual bool validValue(double value) const;
	/// Returns true if the given value is a valid setpoint for this control. False otherwise.
	virtual bool validSetpoint(double value) const;

	/// Returns a string representation of the control.
	virtual QString toString() const;
	/// Returns a string representation of the given mode.
	QString toString(double value) const;

	/// Returns true if the current mode is Continuous, false otherwise. Checks the current values of child controls to determine result.
	bool isContinuous() const;
	/// Returns true if the current mode is SingleShot, false otherwise. Checks the current values of child controls to determine result.
	bool isSingleShot() const;

signals:
	/// Notifier that the scan count control has changed.
	void scanCountControlChanged(AMControl *newControl);
	/// Notifier that the number of scans per buffer control has changed.
	void numberOfScansPerBufferControlChanged(AMControl *newControl);
	/// Notifier that the start scan control has changed.
	void startScanControlChanged(AMControl *newControl);

public slots:
	/// Sets the scan count control.
	void setScanCountControl(AMControl *newControl);
	/// Sets the number of scans per buffer control.
	void setNumberOfScansPerBufferControl(AMControl *newControl);
	/// Sets the start scan control.
	void setStartScanControl(AMControl *newControl);

protected slots:
	/// Handles updating the control's connected state.
	virtual void updateConnected();
	/// Handles updating the control's value.
	virtual void updateValue();
	/// Handles updating the control's 'is moving' state.
	virtual void updateMoving();

	/// Sets the single shot scan count value.
	void setSingleShotScanCountValue(double newValue);
	/// Sets the single shot number of scans per buffer value.
	void setSingleShotNumberOfScansPerBufferValue(double newValue);

protected:
	/// Creates and returns a move action. Returns 0 if not connected.
	virtual AMAction3* createMoveAction(double setpoint);

	/// Creates and returns a move action to Continuous mode.
	AMAction3* createMoveToContinuousModeAction();
	/// Creates and returns a move action to Single Shot mode.
	AMAction3* createMoveToSingleShotModeAction();

protected:
	/// The scan count control.
	AMControl *scanCountControl_;
	/// The number of scans per buffer control.
	AMControl *numberOfScansPerBufferControl_;
	/// The start scan control.
	AMControl *startScanControl_;

	/// The value the scan count control should be set to when entering Single Shot mode.
	double singleShotScanCountValue_;
	/// The value the number of scans control should be set to when entering Single Shot mode.
	double singleShotNumberOfScansPerBufferValue_;
};

#endif // CLSSIS3820SCALERMODECONTROL_H
