#include "CLSAppController.h"

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"
#include "dataman/CLS/CLSDbUpgrade1Pt1.h"

#include "ui/AMMainWindow.h"
#include "util/AMErrorMonitor.h"

CLSAppController::CLSAppController(const QString &beamlineName, QObject *parent) :
    AMAppController(parent)
{
	userConfiguration_ = 0;
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
	detectorPaneIcon_ = ":/utilities-system-monitor.png";
	scanPaneIcon_ = ":/utilities-system-monitor.png";
}

CLSAppController::~CLSAppController()
{
	viewPaneMapping_.clear();
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

void CLSAppController::shutdown()
{
	if (userConfiguration_)
		userConfiguration_->storeToDb(AMDatabase::database("user"));

	AMAppController::shutdown();
}

// ============== implementation of protected slots =====================
void CLSAppController::onUserConfigurationLoadedFromDb()
{
	AMXRFDetector *xrfDetector = CLSBeamline::clsBeamline()->xrfDetector();

	if (userConfiguration_ && xrfDetector) {

		foreach (AMRegionOfInterest *region, userConfiguration_->regionsOfInterest()){
			xrfDetector->addRegionOfInterest(region);
			onRegionOfInterestAdded(region);
		}

		// This is connected here because we want to listen to the detectors for updates, but don't want to double add regions on startup.
		connect(xrfDetector, SIGNAL(addedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestAdded(AMRegionOfInterest*)));
		connect(xrfDetector, SIGNAL(removedRegionOfInterest(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestRemoved(AMRegionOfInterest*)));
		connect(xrfDetector, SIGNAL(regionOfInterestBoundingRangeChanged(AMRegionOfInterest*)), this, SLOT(onRegionOfInterestBoundingRangeChanged(AMRegionOfInterest*)));
	}
}

void CLSAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	onScanEditorCreatedImplementation(editor);
}

void CLSAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	Q_UNUSED(editor)

	AMErrorMon::debug(this, CLS_APPCONTROLLER_INFO_UNIMPLEMENTED_METHOD, "Looks like there is no beamline implementation for onScanEditorCreated(). ");
}

void CLSAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	Q_UNUSED(region)

	AMErrorMon::debug(this, CLS_APPCONTROLLER_INFO_UNIMPLEMENTED_METHOD, "Looks like there is no beamline implementation for onRegionOfInterestAdded(). ");
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

	// create the persistent view
	createPersistentView();

	// Expand the 'General', 'Detectors', and 'Scans' panes.
	mw_->expandAllHeadings();

	// customized user interface implementation for beamline
	setupUserInterfaceImplementation();

	// connect the signal/slot for the scanEditorCreated
	connect(this, SIGNAL(scanEditorCreated(AMGenericScanEditor*)), this, SLOT(onScanEditorCreated(AMGenericScanEditor*)));
}

void CLSAppController::setupUserInterfaceImplementation()
{
	AMErrorMon::debug(this, CLS_APPCONTROLLER_INFO_UNIMPLEMENTED_METHOD, "Looks like there is no special implementation for setupUserInterface(). ");
}

void CLSAppController::addViewToPane(QWidget *view, const QString &viewName, const QString &paneCategoryName, const QString &paneIcon)
{
	if (view) {
		mw_->addPane(view, paneCategoryName, viewName, paneIcon);
	}
}

void CLSAppController::addMainWindowViewToPane(QWidget *view, const QString &viewName, const QString &paneCategoryName, const QString &paneIcon)
{
	if (view) {
		QWidget *pane = AMMainWindow::buildMainWindowPane(viewName, paneIcon, view);
		viewPaneMapping_.insert(view, pane);
		addViewToPane(pane, viewName, paneCategoryName, paneIcon);
	}
}

QString CLSAppController::getStylesheet() const
{
	QString stylesheet = AMAppController::getStylesheet();

	// CLSValueSetpointEditor.

	QFile qss(":/CLS/CLSValueSetpointEditor.qss");

	if (qss.open(QFile::ReadOnly))
		stylesheet.append(QString("\n\n%1").arg(QLatin1String(qss.readAll())));

	qss.close();

	return stylesheet;
}
