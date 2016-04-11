#include "CLSAppController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"

CLSAppController::CLSAppController(const QString &beamlineName, QObject *parent) :
    AMAppController(parent)
{
	clsFacility_ = AMFacility(beamlineName, QString("CLS %1 Beamline").arg(beamlineName), ":/clsIcon.png");
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


