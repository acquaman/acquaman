#include "CLSAppController.h"

#include "dataman/AMRun.h"
#include "dataman/CLS/CLSDbUpgrade1Pt1.h"
#include "beamline/CLS/CLSStorageRing.h"

CLSAppController::CLSAppController(CLSAppController::CLSBeamlineID facilityId, QObject *parent) :
    AMAppController(parent)
{
	facilityId_ = facilityId;

	// Append the AM upgrade 1.7 to the list for the user database
	AMDbUpgrade *am1Pt7UserDb = new CLSDbUpgrade1Pt1(facilityId_, "user", this);
	appendDatabaseUpgrade(am1Pt7UserDb);
	AMDbUpgrade *am1Pt7ActionsDb = new CLSDbUpgrade1Pt1(facilityId_, "actions", this);
	appendDatabaseUpgrade(am1Pt7ActionsDb);
	AMDbUpgrade *am1Pt7ScanActionsDb = new CLSDbUpgrade1Pt1(facilityId_, "scanActions", this);
	appendDatabaseUpgrade(am1Pt7ScanActionsDb);
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
	if (!AMAppController::startupPopulateUserDBTable(userDb))
		return false;

	// initialize the CLS specified facilities
	AMFacility sgm(SGMBeamlineId, beamlineName(SGMBeamlineId), "CLS SGM Beamline", ":/clsIcon.png");
	sgm.storeToDb(userDb);
	AMFacility vespers(VESPERSBeamlineId, beamlineName(VESPERSBeamlineId), "CLS VESPERS Beamline", ":/clsIcon.png");
	vespers.storeToDb(userDb);
	AMFacility reixs(REIXSBeamlineId, beamlineName(REIXSBeamlineId), "CLS REIXS Beamline", ":/clsIcon.png");
	reixs.storeToDb(userDb);
	AMFacility ideas(IDEASBeamlineId, beamlineName(IDEASBeamlineId), "CLS IDEAS Beamline", ":/clsIcon.png");
	ideas.storeToDb(userDb);
	AMFacility bioXASSide(BioXASSideBeamlineId, beamlineName(BioXASSideBeamlineId), "CLS BioXAS Beamline - Side endstation", ":/clsIcon.png");
	bioXASSide.storeToDb(userDb);
	AMFacility bioXASMain(BioXASMainBeamlineId, beamlineName(BioXASMainBeamlineId), "CLS BioXAS Beamline - Main endstation", ":/clsIcon.png");
	bioXASMain.storeToDb(userDb);
	AMFacility bioXASImaging(BioXASImagingBeamlineId, beamlineName(BioXASImagingBeamlineId), "CLS BioXAS Beamline - Imaging endstation", ":/clsIcon.png");
	bioXASImaging.storeToDb(userDb);
	AMFacility sxrmb(SXRMBBeamlineId, beamlineName(SXRMBBeamlineId), "CLS SXRMB Beamline", ":/clsIcon.png");
	sxrmb.storeToDb(userDb);

	// Initialize the AMRun ifnroamtion.
	AMRun existingRun;
	// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
	if (!existingRun.loadFromDb(userDb, 1)){
		AMRun firstRun(beamlineName(facilityId_), (int)facilityId_);
		firstRun.storeToDb(userDb);
	}

	return true;
}

void CLSAppController::initializePeriodicTable()
{
	AMPeriodicTable::table();
}

void CLSAppController::initializeStorageRing()
{
	CLSStorageRing::sr1();
}


