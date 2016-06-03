#ifndef SXRMBHVCONTROL_H
#define SXRMBHVCONTROL_H

#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"

class SXRMBHVControl : public AMPVControl
{
	Q_OBJECT

public:
	/// Constructor
	/*! \param name A unique representative name for this control
  \param basePVname The EPICS channel-access name for the Process Variable
  \param readPVname The EPICS channel-access name for the feedback Process Variable, the real PV name is basePVName + readPVName
  \param writePVname The EPICS channel-access name for the setpoint Process Variable, the real PV name is basePVName + writePVName
  \param powerPVname The EPICS channel-access name for the power on/off Process Variable, the real PV name is basePVName + powerPVName
  \param statusPVname The EPICS channel-access name for the status Process Variable, the real PV name is basePVName + statusPVName
  \param currentPVName The EPICS channel-access name for the measured current Process Variable, the real PV name is basePVName + currentPVName

  \param tolerance The level of accuracy (max. distance between setpoint() and final value()) required for a move() to be successful
  \param moveStartTimeoutSeconds Time allowed after a move() for the Control to first start moving.  If it doesn't, we emit moveFailed(AMControl::TimeoutFailure).
  \param stopPVname is the EPICS channel-access name for the Process Variable used to stop() a move in progress.
  \param stopValue is the value that will be written to the stopPV when stop() is called.
  \param parent QObject parent class
  */
	virtual ~SXRMBHVControl();
	SXRMBHVControl( const QString& name,
					const QString& basePVName,
					const QString& readPVname,
					const QString& writePVname,
					const QString& powerPVname,
					const QString& statusPVname,
					const QString& measuredCurrentPVName,
					QObject* parent = 0,
					double tolerance = AMCONTROL_TOLERANCE_DONT_CARE,
					double moveStartTimeoutSeconds = 2.0,
					int stopValue = 1,
					const QString &description = "");

	int powerStatus() const;
	/// return the value of the current voltage
	double voltage() const;
	/// return the value of the measured current
	double measuredCurrent() const;
	/// return the AMControl for HV power
	AMControl *powerOnOffControl() const;
	/// returns whether we check the measure current or not
	bool hasMeasureCurrent() { return measuredCurrentPV_ != 0; }


	/// override: Indicates that the Control is ready for use.  In this case, means that the readPV is connected and readable.
	virtual bool isConnected() const ;

signals:
	void powerStatusChanged(int);
	void powerPVConnectionTimeout();
	void voltageValueChanged(double);
	void currentValueChanged(double);

public slots:
	/// turn the HV control power on
	void onPowerOn();
	/// turn the HV control power off
	void onPowerOff();

protected slots:

protected:
	/// Pointer to AMPVControl used to write feedback value
	AMControl* powerOnOffControl_;
	/// Pointer to ProcessVariable used to write feedback value
	AMProcessVariable* powerPV_;
	/// Pointer to ProcessVariable used to write feedback value
	AMProcessVariable* statusPV_;
	/// Pointer to ProcessVariable used to write feedback value
	AMProcessVariable* measuredCurrentPV_;

};

#endif // SXRMBHVCONTROL_H
