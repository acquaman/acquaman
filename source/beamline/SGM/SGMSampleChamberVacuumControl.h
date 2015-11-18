#ifndef SGMSAMPLECHAMBERVACUUMCONTROL_H
#define SGMSAMPLECHAMBERVACUUMCONTROL_H

#include "beamline/AMSingleEnumeratedControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMVATValve.h"

class SGMSampleChamberVacuumControl : public AMSingleEnumeratedControl
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SGMSampleChamberVacuumControl(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumControl();

	/// Returns the sample chamber pressure gauge control.
	AMReadOnlyPVControl* pressure() const { return control_; }
	/// Returns the sample chamber turbo pump (pump #5) running state.
	AMSinglePVControl* turboPump5Running() const { return turboPump5Running_; }
	/// Returns the sample chamber turbo pump (pump #6) running state.
	AMSinglePVControl* turboPump6Running() const { return turboPump6Running_; }
	/// Returns the VAT valve.
	SGMVATValve* vatValve() const { return vatValve_; }

signals:
	/// Notifier that the pressure gauge control has changed.
	void pressureChanged(AMReadOnlyPVControl *newControl);
	/// Notifier that the turbo pump (pump #5) running state control has changed.
	void turboPump5RunningChanged(AMSinglePVControl *newControl);
	/// Notifier that the turbo pump (pump #6) running state control has changed.
	void turboPump6RunningChanged(AMSinglePVControl *newControl);
	/// Notifier that the VAT valve control has changed.
	void vatValveChanged(SGMVATValve *newControl);
	/// Notifier that the vacuum options have changed.
	void vacuumOptionsChanged();

public slots:
	/// Sets the sample chamber pressure gauge control.
	void setPressure(AMReadOnlyPVControl *newControl);
	/// Sets the sample chamber turbo pump (pump #5) running state control.
	void setTurboPump5Running(AMSinglePVControl *newControl);
	/// Sets the sample chamber turbo pump (pump #6) running state control.
	void setTurboPump6Running(AMSinglePVControl *newControl);
	/// Sets the sample chamber VAT valve control.
	void setVATValve(SGMVATValve *newControl);

protected slots:
	/// Updates the connected state. Reimplemented to consider all of the member controls, not just the pressure/base control.
	virtual void updateConnected();

	/// Adds a vacuum option.
	virtual void addVacuumOption(int index, const QString &optionString, double valueMin, double valueMax);
	/// Removes a vacuum option.
	virtual void removeVacuumOption(int index);

protected:
	/// The sample chamber turbo pump (pump #5) running state control.
	AMSinglePVControl *turboPump5Running_;
	/// The sample chamber turbo pump (pump #6) running state control.
	AMSinglePVControl *turboPump6Running_;
	/// The sample chamber VAT valve control.
	SGMVATValve *vatValve_;
};

#endif // SGMSAMPLECHAMBERVACUUMCONTROL_H
