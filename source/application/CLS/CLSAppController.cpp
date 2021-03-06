#include "CLSAppController.h"

#include "beamline/AMBeamline.h"
#include "beamline/CLS/CLSStorageRing.h"

#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/CLS/CLSDbUpgrade1Pt1.h"
#include "dataman/CLS/CLSDbUpgrade1Pt2.h"
#include "dataman/CLS/CLSDbUpgrade1Pt3.h"
#include "dataman/CLS/CLSUserConfiguration.h"

#include "ui/AMMainWindow.h"
#include "util/AMErrorMonitor.h"

CLSAppController::CLSAppController(const QString &beamlineName, QObject *parent) :
    AMAppController(parent)
{
	userConfiguration_ = 0;
	clsFacility_ = AMFacility(beamlineName, QString("CLS %1 Beamline").arg(beamlineName), ":/clsIcon.png");

	// Append the CLS upgrade to the list for the user database
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(beamlineName, "user", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(beamlineName, "actions", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt1(beamlineName, "scanActions", this));

	appendDatabaseUpgrade(new CLSDbUpgrade1Pt2(beamlineName, "user", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt2(beamlineName, "actions", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt2(beamlineName, "scanActions", this));

	appendDatabaseUpgrade(new CLSDbUpgrade1Pt3("user", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt3("actions", this));
	appendDatabaseUpgrade(new CLSDbUpgrade1Pt3("scanActions", this));

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
void CLSAppController::onScanEditorCreated(AMGenericScanEditor *editor)
{
	onScanEditorCreatedImplementation(editor);
}

void CLSAppController::onScanEditorCreatedImplementation(AMGenericScanEditor *editor)
{
	Q_UNUSED(editor)

	AMErrorMon::debug(this, CLS_APPCONTROLLER_INFO_UNIMPLEMENTED_METHOD, "Looks like there is no special implementation for onScanEditorCreated(). ");
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

void CLSAppController::registerDBClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSUserConfiguration>();
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

void CLSAppController::addMainWindowPane(QWidget *view, const QString &viewName, const QString &paneCategoryName, const QString &paneIcon)
{
	if (view)
		mw_->addPane(view, paneCategoryName, viewName, paneIcon);
}

void CLSAppController::addMainWindowView(QWidget *view, const QString &viewName, const QString &paneCategoryName, const QString &paneIcon)
{
	if (view) {
		QWidget *pane = AMMainWindow::buildMainWindowPane(viewName, paneIcon, view);
		viewPaneMapping_.insert(view, pane);
		addMainWindowPane(pane, viewName, paneCategoryName, paneIcon);
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
