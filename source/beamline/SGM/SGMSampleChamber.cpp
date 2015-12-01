#include "SGMSampleChamber.h"
#include "beamline/SGM/SGMVATValve.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

SGMSampleChamber::SGMSampleChamber(QObject *parent) :
	AMConnectedControl("SGMSampleChamber", "", parent)
{
	// Set up the pressure control.

	pressure_ = new AMReadOnlyPVControl("sampleChamberPressure", "FRG1611-4-I10-01:pressure:fbk", this);
	addChildControl(pressure_);

	// Set up the door control.

	// Set up the vat valve control.

	vatValve_ = new SGMVATValve("sampleChamberVATValve", "VVR1611-4-I10-11", this);
	addChildControl(vatValve_);

	// Set up the turbos control set.

	turbos_ = new AMControlSet(this);
	turbos_->addControl(new SGMTurboPump("TMP1611-4-I10-05", "TMP1611-4-I10-05", this));
	turbos_->addControl(new SGMTurboPump("TMP1611-4-I10-06", "TMP1611-4-I10-06", this));

	connect( turbos_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Set up the vacuum control.

	vacuum_ = new SGMSampleChamberVacuum("sampleChamberVacuum", this);
	vacuum_->setPressure(pressure_);
	addChildControl(vacuum_);
}

SGMSampleChamber::~SGMSampleChamber()
{

}

bool SGMSampleChamber::isConnected() const
{
	bool connected = (
				pressure_ && pressure_->isConnected() &&
				vatValve_ && vatValve_->isConnected() &&
				turbos_ && turbos_->isConnected() &&
				vacuum_ && vacuum_->isConnected()
				);

	return connected;
}
