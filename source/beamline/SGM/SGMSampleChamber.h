#ifndef SGMSAMPLECHAMBER_H
#define SGMSAMPLECHAMBER_H

#include "beamline/AMConnectedControl.h"

class AMPVControl;
class AMControlSet;
class AMReadOnlyPVControl;
class SGMVATValve;
class SGMSampleChamberVacuum;
class SGMTurboPump;
class SGMRoughingPump;

class SGMSampleChamber : public AMConnectedControl
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamber(QObject *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamber();

	/// Returns true if connected, false otherwise.
	virtual bool isConnected() const;

	/// The door.
	AMReadOnlyPVControl* doorStatus() const { return doorStatus_; }
	/// The pressure.
	AMReadOnlyPVControl* pressure() const { return pressure_; }
	/// The ion gauge running status.
	AMPVControl* ionGaugeRunning() const { return ionGaugeRunning_; }
	/// The VAT valve.
	SGMVATValve* vatValve() const { return vatValve_; }
	/// The roughing pump.
	SGMRoughingPump* roughingPump() const { return roughingPump_; }
	/// The turbos.
	AMControlSet* turbos() const { return turbos_; }
	/// The vacuum.
	SGMSampleChamberVacuum* vacuum() const { return vacuum_; }
        /// The sample chamber light
        AMControl* chamberLight() const { return chamberLight_; }

protected:
	/// The sample chamber door status control.
	AMReadOnlyPVControl *doorStatus_;
	/// The sample chamber pressure control.
	AMReadOnlyPVControl *pressure_;
	/// The sample chamber pressure ion gauge running control.
	AMPVControl *ionGaugeRunning_;
	/// The sample chamber VAT valve.
	SGMVATValve *vatValve_;
	/// The roughing pump.
	SGMRoughingPump *roughingPump_;
	/// The turbos.
	AMControlSet *turbos_;
	/// The sample chamber vacuum control.
	SGMSampleChamberVacuum *vacuum_;
        /// The sample chamber light
        AMControl* chamberLight_;
};

#endif // SGMSAMPLECHAMBER_H
