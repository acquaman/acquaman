#include "CLSAppController.h"

#include "dataman/AMRun.h"

#include "beamline/CLS/CLSStorageRing.h"

CLSAppController::CLSAppController(CLSAppController::CLSBeamlineID facilityId, QObject *parent) :
    AMAppController(parent)
{
	facilityId_ = facilityId;
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
		registerBeamlineDBClasses();
		setupBeamlineExporterOptions();
		setupAcquamanUserInterface();
		setupBeamlineSignalConnections();

		return true;
	}

	return false;
}

bool CLSAppController::startupPopulateUserDBTable(AMDatabase* userDb)
{
	// initialize the facility of the current beamline
	QString facilityName = beamlineName(facilityId_);
	AMFacility amfacility(facilityName, QString("CLS %1 Beamline").arg(facilityName), ":/clsIcon.png");
	amfacility.storeToDb(userDb);

	// Initialize the AMRun ifnroamtion.
	AMRun existingRun;
	// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
	if (!existingRun.loadFromDb(userDb, 1)){
		AMRun firstRun(facilityName);
		firstRun.storeToDb(userDb);
	}

	return true;
}

QString CLSAppController::beamlineName(CLSAppController::CLSBeamlineID beamline) {
	QString name;
	switch (beamline) {
	case SGMBeamlineId:
		name = "SGM";
		break;
	case VESPERSBeamlineId:
		name = "VESPERS";
		break;
	case REIXSBeamlineId:
		name = "REIXS";
		break;
	case IDEASBeamlineId:
		name = "IDEAS";
		break;
	case BioXASMainBeamlineId:
		name = "BioXASMain";
		break;
	case BioXASSideBeamlineId:
		name = "BioXASSide";
		break;
	case BioXASImagingBeamlineId:
		name = "BioXASImaging";
		break;
	case SXRMBBeamlineId:
		name = "SXRMB";
		break;
	default:
		name = "Unknown";
		break;
	}

	return name;
}

void CLSAppController::initializePeriodicTable()
{
	AMPeriodicTable::table();
}

void CLSAppController::initializeStorageRing()
{
	CLSStorageRing::sr1();
}


