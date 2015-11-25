#ifndef SGMSAMPLECHAMBERVACUUMCONTROL_H
#define SGMSAMPLECHAMBERVACUUMCONTROL_H

#include "source/beamline/AMSingleEnumeratedControl.h"
#include "source/beamline/AMControl.h"
#include "source/beamline/AMPVControl.h"
#include "source/beamline/SGM/SGMVATValve.h"
#include "source/beamline/SGM/SGMTurboPump.h"
#include "source/beamline/AMControlSet.h"

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

	/// Returns true if this control can move right now. Reimplemented, as the pressure (base) control is read only.
	virtual bool canMove() const;

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
	/// The sample chamber turbo pumps.
	AMControlSet *turbos_;
	/// The sample chamber VAT valve control.
	SGMVATValve *vatValve_;
};

#endif // SGMSAMPLECHAMBERVACUUMCONTROL_H
