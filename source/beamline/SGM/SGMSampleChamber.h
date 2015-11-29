#ifndef SGMSAMPLECHAMBER_H
#define SGMSAMPLECHAMBER_H

#include "beamline/AMConnectedControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

class SGMVATValve;
class SGMSampleChamberVacuum;
class SGMTurboPump;

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

	/// The vacuum.
	SGMSampleChamberVacuum* vacuum() const { return vacuum_; }
	/// The door.
	AMReadOnlyPVControl* door() const { return 0; }
	/// The pressure.
	AMReadOnlyPVControl* pressure() const { return pressure_; }
	/// The VAT valve.
	SGMVATValve* vatValve() const { return vatValve_; }
	/// The roughing pump.
	AMControl* roughingPump() const { return 0; }
	/// The turbos.
	AMControlSet* turbos() const { return turbos_; }

protected:
	/// The sample chamber vacuum control.
	SGMSampleChamberVacuum *vacuum_;
	/// The sample chamber pressure control.
	AMReadOnlyPVControl *pressure_;
	/// The sample chamber VAT valve.
	SGMVATValve *vatValve_;
	/// The turbos.
	AMControlSet *turbos_;
};

#endif // SGMSAMPLECHAMBER_H
