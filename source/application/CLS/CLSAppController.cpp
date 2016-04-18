#include "CLSAppController.h"

#include "dataman/AMRun.h"

#include "beamline/AMBeamline.h"
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
		registerClasses();
		setupExporterOptions();
//		setupUserConfiguration();
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

bool CLSAppController::startupPopulateUserDBTable(AMDatabase* userDb)
{
	if (!AMAppController::startupPopulateUserDBTable(userDb))
		return false;

	// initialize the CLS specified facilities
	AMFacility sgm(beamlineName(SGMBeamlineId), "CLS SGM Beamline", ":/clsIcon.png");
	sgm.storeToDb(userDb);
	AMFacility vespers(beamlineName(VESPERSBeamlineId), "CLS VESPERS Beamline", ":/clsIcon.png");
	vespers.storeToDb(userDb);
	AMFacility reixs(beamlineName(REIXSBeamlineId), "CLS REIXS Beamline", ":/clsIcon.png");
	reixs.storeToDb(userDb);
	AMFacility ideas(beamlineName(IDEASBeamlineId), "CLS IDEAS Beamline", ":/clsIcon.png");
	ideas.storeToDb(userDb);
	AMFacility bioXASSide(beamlineName(BioXASSideBeamlineId), "CLS BioXAS Beamline - Side endstation", ":/clsIcon.png");
	bioXASSide.storeToDb(userDb);
	AMFacility bioXASMain(beamlineName(BioXASMainBeamlineId), "CLS BioXAS Beamline - Main endstation", ":/clsIcon.png");
	bioXASMain.storeToDb(userDb);
	AMFacility bioXASImaging(beamlineName(BioXASImagingBeamlineId), "CLS BioXAS Beamline - Imaging endstation", ":/clsIcon.png");
	bioXASImaging.storeToDb(userDb);
	AMFacility sxrmb(beamlineName(SXRMBBeamlineId), "CLS SXRMB Beamline", ":/clsIcon.png");
	sxrmb.storeToDb(userDb);
	AMFacility pgm(beamlineName(PGMBeamlineId), "CLS PGM Beamline", ":/clsIcon.png");
	pgm.storeToDb(userDb);

	// Initialize the AMRun ifnroamtion.
	AMRun existingRun;
	// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
	if (!existingRun.loadFromDb(userDb, 1)){
		AMRun firstRun(beamlineName(facilityId_), (int)facilityId_);
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
	case PGMBeamlineId:
		name = "PGM";
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


