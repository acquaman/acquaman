#include "SGMSampleChamber.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"
#include "beamline/SGM/SGMVATValve.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/SGM/SGMRoughingPump.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

SGMSampleChamber::SGMSampleChamber(QObject *parent) :
	AMConnectedControl("SGMSampleChamber", "", parent)
{
	// Set up the door status control.

	doorStatus_ = new AMReadOnlyPVControl("sampleChamberDoorStatus", "ES1611-4-I10-02:TankClosed", this);
	addChildControl(doorStatus_);

	// Set up the pressure control.

	pressure_ = new AMReadOnlyPVControl("sampleChamberPressure", "FRG1611-4-I10-01:pressure:fbk", this);
	addChildControl(pressure_);

	// Set up the ion gauge.

	ionGaugeRunning_ = new AMPVControl("sampleChamberIonGauge", "FRG1611-4-I10-01:gauge:ion:rbk", "FRG1611-4-I10-01:gauge:ion", "", this);
	addChildControl(ionGaugeRunning_);

	// Set up the vat valve control.

	vatValve_ = new SGMVATValve("sampleChamberVATValve", "VVR1611-4-I10-11", this);
	addChildControl(vatValve_);

	// Set up the turbos control set.

	turbos_ = new AMControlSet(this);
	turbos_->addControl(new SGMTurboPump("TMP1611-4-I10-05", "TMP1611-4-I10-05", this));
	turbos_->addControl(new SGMTurboPump("TMP1611-4-I10-06", "TMP1611-4-I10-06", this));

	connect( turbos_, SIGNAL(connected(bool)), this, SLOT(updateConnected()) );

	// Set up the roughing pump.

	roughingPump_ = new SGMRoughingPump("PVP1611-4-I10-03", "PVP1611-4-I10-03", this);
	addChildControl(roughingPump_);

	// Set up the vacuum control.

	vacuum_ = new SGMSampleChamberVacuum("sampleChamberVacuum", this);
	vacuum_->setPressure(pressure_);
	addChildControl(vacuum_);

        // Set up the chamber light.
        chamberLight_ = new AMSinglePVControl("sampleChamberLight","ILC1611-4-I10-02",this, 1, 10);
}

SGMSampleChamber::~SGMSampleChamber()
{

}

bool SGMSampleChamber::isConnected() const
{
	bool connected = (
				doorStatus_ && doorStatus_->isConnected() &&
				pressure_ && pressure_->isConnected() &&
				vatValve_ && vatValve_->isConnected() &&
				turbos_ && turbos_->isConnected() &&
				roughingPump_ && roughingPump_->isConnected() &&
				vacuum_ && vacuum_->isConnected()
				);

	return connected;
}
