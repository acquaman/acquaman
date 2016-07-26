#include "CLSBeamlineStatus.h"

#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSValves.h"

CLSBeamlineStatus::CLSBeamlineStatus(const QString &name, QObject *parent) :
	CLSBiStateGroup(name, parent)
{
	beamlineStatusPVControl_ = 0;
	shuttersControlSet_ = new AMControlSet(this);
	valvesControlSet_ = new AMControlSet(this);
	mirrorMaskControlSet_ = new AMControlSet(this);
	monoMaskControlSet_ = new AMControlSet(this);

	// Setup the basic value options.
	addOption(On, "Beam is ON", true);
	addOption(Off, "Beam is OFF", true);

	connect(this, SIGNAL(connected(bool)), this, SLOT(updateBeamlineStatus()));
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(updateBeamlineStatus()));
	connect(this, SIGNAL(componentsChanged()), this, SLOT(updateBeamlineStatus()));
}

CLSBeamlineStatus::~CLSBeamlineStatus()
{
	clearComponents();
}

/// ========================  public memembers =====================
bool CLSBeamlineStatus::isOn() const
{
	if (beamlineStatusPVControl_) {
		return (isConnected() && isChildState1(beamlineStatusPVControl_));
	} else {
		return (isConnected() && areAllChildrenState1());
	}
}

bool CLSBeamlineStatus::isOff() const
{
	return isConnected() && !isOn();
}

AMControl * CLSBeamlineStatus::beamlineStatusPVControl()
{
	return beamlineStatusPVControl_ ;
}

QList<AMControl*> CLSBeamlineStatus::componentsInBeamOnState() const
{
	return childrenInState1();
}

QList<AMControl*> CLSBeamlineStatus::componentsNotInBeamOnState() const
{
	return childrenNotInState1();
}

void CLSBeamlineStatus::setBeamlineStatusPVControl(AMControl *control, double beamOnValue)
{
	if ( addComponent(control, beamOnValue) ) {
		beamlineStatusPVControl_ = control;

		updateBeamlineStatus();
	}
}

bool CLSBeamlineStatus::addShutterControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		shuttersControlSet_->addControl(newControl);

	return result;
}

bool CLSBeamlineStatus::addValveControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		valvesControlSet_->addControl(newControl);

	return result;
}

bool CLSBeamlineStatus::addMirrorMaskControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		mirrorMaskControlSet_->addControl(newControl);

	return result;
}

bool CLSBeamlineStatus::addMonoMaskControl(AMControl *newControl, double beamOnValue)
{
	bool result = addComponent(newControl, beamOnValue);
	if (result)
		monoMaskControlSet_->addControl(newControl);

	return result;
}

/// ========================  protected memembers =====================

void CLSBeamlineStatus::updateBeamlineStatus()
{
	emit beamStatusChanged(isOn());
}

bool CLSBeamlineStatus::removeComponent(AMControl *control)
{
	bool result = removeBiStateControl(control);

	if (result) {
		if (isShutterControl(control))
			shuttersControlSet_->removeControl(control);
		else if (isValveControl(control))
			valvesControlSet_->removeControl(control);
		else if (isMirrorMaskControl(control))
			mirrorMaskControlSet_->removeControl(control);
		else if (isMonoMaskControl(control))
			monoMaskControlSet_->removeControl(control);
		else if (beamlineStatusPVControl_ == control)
			beamlineStatusPVControl_ = 0;

		emit componentsChanged();
	}

	return result;
}

bool CLSBeamlineStatus::clearComponents()
{
	bool result = clearBiStateControls();

	if (result) {
		shuttersControlSet_->clear();
		valvesControlSet_->clear();
		mirrorMaskControlSet_->clear();
		monoMaskControlSet_->clear();
		beamlineStatusPVControl_ = 0;

		emit componentsChanged();
	}

	return result;
}

bool CLSBeamlineStatus::addComponent(AMControl *newControl, double beamOnValue)
{
	bool result = addBiStateControl(newControl, beamOnValue);

	if (result)
		emit componentsChanged();

	return result;
}

int CLSBeamlineStatus::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (isOn())
		result = On;
	else if (isOff())
		result = Off;

	return result;
}
