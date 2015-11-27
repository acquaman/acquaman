#include "SGMSampleChamber.h"
#include "beamline/SGM/SGMVATValve.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

SGMSampleChamber::SGMSampleChamber(QObject *parent) :
	AMConnectedControl("SGMSampleChamber", "", parent)
{
	pressure_ = new AMReadOnlyPVControl("sampleChamberPressure", "FRG1611-4-I10-01:pressure:fbk", this);
	addChildControl(pressure_);

	vatValve_ = new SGMVATValve("sampleChamberVATValve", "VVR1611-4-I10-11", this);
	addChildControl(vatValve_);

	turbos_ = new AMControlSet(this);
	turbos_->addControl(new SGMTurboPump("TMP1611-4-I10-05", "TMP1611-4-I10-05", this));
	turbos_->addControl(new SGMTurboPump("TMP1611-4-I10-06", "TMP1611-4-I10-06", this));

	connect( turbos_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	vacuum_ = new SGMSampleChamberVacuum(this);
	vacuum_->setPressure(pressure_);
	vacuum_->setVATValve(vatValve_);
	vacuum_->setTurbos(turbos_);
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
