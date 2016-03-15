#include "CLSAppController.h"

#include "dataman/AMRun.h"
#include "dataman/CLS/CLSDbUpgrade1Pt1.h"
#include "beamline/CLS/CLSStorageRing.h"

QString CLSAppController::beamlineName(int beamlineId)
{
	QString name;
	switch (beamlineId) {
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

CLSAppController::CLSAppController(CLSAppController::CLSBeamlineID facilityId, QObject *parent) :
    AMAppController(parent)
{
	facilityId_ = facilityId;

	// Append the CLS upgrade 1.1 to the list for the user database
	AMDbUpgrade *cls1Pt1UserDb = new CLSDbUpgrade1Pt1(facilityId_, "user", this);
	appendDatabaseUpgrade(cls1Pt1UserDb);
	AMDbUpgrade *cls1Pt1ActionsDb = new CLSDbUpgrade1Pt1(facilityId_, "actions", this);
	appendDatabaseUpgrade(cls1Pt1ActionsDb);
	AMDbUpgrade *cls1Pt1ScanActionsDb = new CLSDbUpgrade1Pt1(facilityId_, "scanActions", this);
	appendDatabaseUpgrade(cls1Pt1ScanActionsDb);
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
		initializeBeamlineComponents();
		registerBeamlineDBClasses();
		setupDataExporterOptions();
		setupUserInterface();
		setupBeamlineSignalConnections();

		return true;
	}

	return false;
}

bool CLSAppController::startupPopulateUserDBTable(AMDatabase* userDb)
{
	// AMFacility will contain only one facility for each beamline and it will be created for the first time run
	AMFacility clsFacility;
	if (!clsFacility.loadFromDb(userDb, 1)) {
		// initialize the facility of the current beamline
		QString facilityName = beamlineName(facilityId_);
		AMFacility newCLSfacility(facilityName, QString("CLS %1 Beamline").arg(facilityName), ":/clsIcon.png");
		newCLSfacility.storeToDb(userDb);

		// initialize the default AMRun information
		AMRun firstRun(facilityName);
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


