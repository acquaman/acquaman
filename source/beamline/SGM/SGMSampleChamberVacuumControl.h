#ifndef SGMSAMPLECHAMBERVACUUMCONTROL_H
#define SGMSAMPLECHAMBERVACUUMCONTROL_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMVATValve.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/AMControlSet.h"

#define SGMSAMPLECHAMBERVACUUMCONTROL_TURBO_TIMEOUT 10
#define SGMSAMPLECHAMBERVACUUMCONTROL_VATVALVE_TIMEOUT 10

class SGMSampleChamberVacuumControl : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumControl();

	/// Returns the sample chamber pressure gauge control.
	AMControl* pressure() const { return control_; }
	/// Returns the sample chamber turbo pumps.
	AMControlSet* turbos() const { return turbos_; }
	/// Returns the VAT valve.
	SGMVATValve* vatValve() const { return vatValve_; }

signals:
	/// Notifier that the pressure gauge control has changed.
	void pressureChanged(AMControl *newControl);
	/// Notifier that the chamber turbo pumps have changed.
	void turbosChanged(AMControlSet *newTurbos);
	/// Notifier that the VAT valve control has changed.
	void vatValveChanged(SGMVATValve *newControl);
	/// Notifier that the vacuum options have changed.
	void vacuumOptionsChanged();

public slots:
	/// Sets the sample chamber pressure gauge control.
	void setPressure(AMControl *newControl);
	/// Sets the sample chamber turbo pumps.
	void setTurbos(AMControlSet *newControls);
	/// Sets the sample chamber VAT valve control.
	void setVATValve(SGMVATValve *newControl);

protected slots:
	/// Updates the connected state. Reimplemented to consider all of the member controls, not just the pressure/base control.
	virtual void updateConnected();

	/// Adds a vacuum option.
	virtual bool addVacuumOption(int index, const QString &optionString, double valueMin, double valueMax);
	/// Removes a vacuum option.
	virtual bool removeVacuumOption(int index);

protected:
	/// Returns a new action that turns off the given turbo pump. Returns 0 if no valid turbo pump is given.
	AMAction3* turnOffTurbo(SGMTurboPump *turbo);
	/// Returns a new action that checks that the given turbo pump is off. Returns 0 if no valid turbo pump is given.
	AMAction3* checkTurboOff(SGMTurboPump *turbo);
	/// Returns a new action that turns on the given turbo pump. Returns 0 if no valid turbo pump is given.
	AMAction3* turnOnTurbo(SGMTurboPump *turbo);
	/// Returns a new action that checks that the given turbo pump is on. Returns 0 if no valid turbo pump is given.
	AMAction3* checkTurboOn(SGMTurboPump *turbo);

	/// Returns a new action that turns off the chamber turbo pumps. Returns 0 if turbos are invalid.
	AMAction3* turnOffChamberTurbos();
	/// Returns a new action that checks that the chamber turbo pumps are off. Returns 0 if the turbos are invalid.
	AMAction3* checkChamberTurbosOff();
	/// Returns a new action that turns on the chamber turbo pumps. Returns 0 if the turbos are invalid.
	AMAction3* turnOnChamberTurbos();
	/// Returns a new action that checks that the chamber turbo pumps are on. Returns 0 if the turbos are invlaid.
	AMAction3* checkChamberTurbosOn();

	/// Returns a new action that turns off the chamber roughing pump. Returns 0 if roughing pump is invalid.
	AMAction3* turnOffChamberRoughingPump() { return 0; }
	/// Returns a new action that checks that the chamber roughing pump is off. Returns 0 if roughing pump is invalid.
	AMAction3* checkChamberRoughingPumpOff() { return 0; }
	/// Returns a new action that turns on the chamber roughing pump. Returns 0 if no valid turbo pump is given.
	AMAction3* turnOnChamberRoughingPump() { return 0; }
	/// Returns a new action that checks that the chamber roughing pump is on. Returns 0 if roughing pump is invalid.
	AMAction3* checkChamberRoughingPumpOn() { return 0; }

	/// Returns a new action that closes the VAT valve, at the given speed. Returns 0 if no valid valve is given.
	AMAction3* closeVATValve(double speed);
	/// Returns a new action that checks that the VAT valve is closed. Returns 0 if no valid valve is given.
	AMAction3* checkVATValveClosed();
	/// Returns a new action that opens the VAT valve, at the given speed. Returns 0 if no valid valve is given.
	AMAction3* openVATValve(double speed);
	/// Returns a new action that checks that the VAT valve is open. Returns 0 if no valid valve is given.
	AMAction3* checkVATValveOpen();

	/// Returns a new action that checks that the chamber door is closed. Returns 0 if no valid door control is found.
	AMAction3* checkDoorClosed() { return 0; }
	/// Returns a new action that checks that the chamber door is open. Returns 0 if no valid door control found.
	AMAction3* checkDoorOpen() { return 0; }

protected:
	/// The sample chamber turbo pumps.
	AMControlSet *turbos_;
	/// The sample chamber VAT valve control.
	SGMVATValve *vatValve_;
};

#endif // SGMSAMPLECHAMBERVACUUMCONTROL_H
