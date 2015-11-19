#ifndef SGMSAMPLECHAMBERVACUUMCONTROL_H
#define SGMSAMPLECHAMBERVACUUMCONTROL_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMVATValve.h"
#include "beamline/SGM/SGMTurboPump.h"

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
	/// Returns the sample chamber turbo pump (pump #5).
	SGMTurboPump* turbo5() const { return turbo5_; }
	/// Returns the sample chamber turbo pump (pump #6).
	SGMTurboPump* turbo6() const { return turbo6_; }
	/// Returns the VAT valve.
	SGMVATValve* vatValve() const { return vatValve_; }

signals:
	/// Notifier that the pressure gauge control has changed.
	void pressureChanged(AMControl *newControl);
	/// Notifier that the turbo pump (pump #5) running state control has changed.
	void turbo5Changed(SGMTurboPump *newControl);
	/// Notifier that the turbo pump (pump #6) running state control has changed.
	void turbo6Changed(SGMTurboPump *newControl);
	/// Notifier that the VAT valve control has changed.
	void vatValveChanged(SGMVATValve *newControl);
	/// Notifier that the vacuum options have changed.
	void vacuumOptionsChanged();

public slots:
	/// Sets the sample chamber pressure gauge control.
	void setPressure(AMReadOnlyPVControl *newControl);
	/// Sets the sample chamber turbo pump (pump #5) running state control.
	void setTurbo5(SGMTurboPump *newControl);
	/// Sets the sample chamber turbo pump (pump #6) running state control.
	void setTurbo6(SGMTurboPump *newControl);
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
	/// The sample chamber turbo pump (pump #5).
	SGMTurboPump *turbo5_;
	/// The sample chamber turbo pump (pump #6).
	SGMTurboPump *turbo6_;
	/// The sample chamber VAT valve control.
	SGMVATValve *vatValve_;
};

#endif // SGMSAMPLECHAMBERVACUUMCONTROL_H
