#include "BioXASXIAFiltersActuator.h"

BioXASXIAFiltersActuator::BioXASXIAFiltersActuator(const QString &deviceName, const QString &baseName, QObject *parent) :
	BioXASXIAFiltersActuatorControl(deviceName, parent)
{
	setStatus(new AMReadOnlyPVControl(name()+"Status", baseName+":control", this));
	setIn(new AMSinglePVControl(name()+"In", baseName+":OprInsert", this));
	setOut(new AMSinglePVControl(name()+"Out", baseName+":OprExtract", this));
}

BioXASXIAFiltersActuator::~BioXASXIAFiltersActuator()
{

}
