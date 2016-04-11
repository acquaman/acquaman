#include "CLSAppController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "dataman/CLS/CLSDbUpgrade1Pt1.h"

CLSAppController::CLSAppController(const QString &beamlineName, QObject *parent) :
    AMAppController(parent)
{
	clsFacility_ = AMFacility(beamlineName, QString("CLS %1 Beamline").arg(beamlineName), ":/clsIcon.png");

	// Append the CLS upgrade 1.1 to the list for the user database
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(facilityId_, "user", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(facilityId_, "actions", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(facilityId_, "scanActions", this));
}

CLSAppController::~CLSAppController()
{

}

bool CLSAppController::startup()
{
	if (AMAppController::startup()) {

		// Initialize singleton objects.
		initializePeriodicTable();
		initializeStorageRing();

		// initialize beamline specific resources
		initializeBeamline();
		registerClasses();
		setupExporterOptions();
		setupUserConfiguration();
		setupUserInterface();
		makeConnections();

		return true;
	}

	return false;
}

void CLSAppController::shutdown()
{
	AMAppController::shutdown();
}

void CLSAppController::initializePeriodicTable()
{
	AMPeriodicTable::table();
}

void CLSAppController::initializeStorageRing()
{
	CLSStorageRing::sr1();
}


