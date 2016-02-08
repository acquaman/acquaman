#include "BioXASXIAFiltersActuator.h"

BioXASXIAFiltersActuator::BioXASXIAFiltersActuator(const QString &deviceName, const QString &baseName, QObject *parent) :
	BioXASXIAFiltersActuatorControl(deviceName, parent)
{
	// Setup actuator controls.

	setStatusControl(new AMReadOnlyPVControl(name()+"Status", baseName+":control", this));
	addFilterState(Out, stateToString(Out), new AMSinglePVControl(name()+"Out", baseName+":OprExtract", this));
	addFilterState(In, stateToString(In), new AMSinglePVControl(name()+"In", baseName+":OprInsert", this));
}

BioXASXIAFiltersActuator::~BioXASXIAFiltersActuator()
{

}

QString BioXASXIAFiltersActuator::stateToString(double state)
{
	QString result = "Unknown";

	switch (int(state)) {
	case In:
		result = "In";
		break;
	case Out:
		result = "Out";
		break;
	default:
		break;
	}

	return result;
}
