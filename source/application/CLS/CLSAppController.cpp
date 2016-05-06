#include "CLSAppController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "dataman/CLS/CLSDbUpgrade1Pt1.h"

#include "ui/AMMainWindow.h"
#include "util/AMErrorMonitor.h"

CLSAppController::CLSAppController(const QString &beamlineName, QObject *parent) :
    AMAppController(parent)
{
	clsFacility_ = AMFacility(beamlineName, QString("CLS %1 Beamline").arg(beamlineName), ":/clsIcon.png");

	// Append the CLS upgrade 1.1 to the list for the user database
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(beamlineName, "user", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(beamlineName, "actions", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(beamlineName, "scanActions", this));

	// member variables
	generalPaneCategeryName_ = "General";
	detectorPaneCategoryName_ = "Detectors";
	scanPaneCategoryName_ = "Scans";

	generalPaneIcon_ = ":/system-software-update.png";
	detectorsPaneIcon_ = ":/utilities-system-monitor.png";
	scansPaneIcon_ = ":/utilities-system-monitor.png";
}

CLSAppController::~CLSAppController()
{

}

bool CLSAppController::startup()
{
	if (!setupDataFolder())
		return false;

	if (AMAppController::startup()) {

		// Initialize singleton objects.
		initializePeriodicTable();
		initializeStorageRing();

		// initialize beamline specific resources
		initializeBeamline();
		registerDBClasses();
		registerExporterOptions();
		setupScanConfigurations();
		setupUserInterface();
		setupUserConfiguration();

		// Ensuring we automatically switch scan editors for new scans.
		setAutomaticBringScanEditorToFront(true);

		return true;
	}

	return false;
}

// ============== implementation of protected slots =====================
void CLSAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	onScanEditorCreatedImplementation(editor);
}

void CLSAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	Q_UNUSED(editor)

	AMErrorMon::information(this, CLS_APPCONTROLLER_INFO_UNIMPLEMENTED_METHOD, "Looks like there is no special implementation for onScanEditorCreated(). ");
}

// =============== implementation of protected functions =================
void CLSAppController::initializePeriodicTable()
{
	AMPeriodicTable::table();
}

void CLSAppController::initializeStorageRing()
{
	CLSStorageRing::sr1();
}

void CLSAppController::setupUserInterface()
{
	// create the persistent view
	createPersistentView();

	// Create panes in the main window:
	////////////////////////////////////

	// General heading
	mw_->insertHeading(generalPaneCategeryName_, 0);
	createGeneralPanes();

	// Detectors heading
	mw_->insertHeading(detectorPaneCategoryName_, 1);
	createDetectorPanes();

	// Scans heading
	mw_->insertHeading(scanPaneCategoryName_, 2);
	createScanConfigurationPanes();

	// customized user interface implementation for beamline
	setupUserInterfaceImplementation();

	// connect the signal/slot for the scanEditorCreated
	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));
}

void CLSAppController::setupUserInterfaceImplementation()
{
	AMErrorMon::information(this, CLS_APPCONTROLLER_INFO_UNIMPLEMENTED_METHOD, "Looks like there is no special implementation for setupUserInterface(). ");
}
