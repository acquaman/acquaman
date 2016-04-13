#include "AMTESTAppController.h"

#include "appController/AMDSClientAppController.h"
#include "ClientRequest/AMDSClientRequestSupport.h"
#include "DataElement/AMDSEventDataSupport.h"
#include "DataHolder/AMDSDataHolderSupport.h"

#include "ui/TimeEvolutionStripTool/AMTESTCentralWidgetView.h"
#include "ui/TimeEvolutionStripTool/AMTESTMainWindow.h"
#include "util/TimeEvolutionStripTool/AMTESTStripTool.h"

#include "util/AMErrorMonitor.h"

AMTESTAppController::AMTESTAppController(QObject *parent)
	: QObject(parent)
{
	isRunning_ = false;
	mainWindow_ = 0;
}

AMTESTAppController::~AMTESTAppController()
{
	mainWindow_->deleteLater();
}

bool AMTESTAppController::startup()
{
	AMErrorMon::information(this, AMTESTAPPCONTROLER_STARTING, "Time Evolution Strip Tool is now starting...");

	// Initialize model.
	AMTESTStripTool::stripTool();

	registerClasses();
	initializeDataServer();
	setupUI();

	isRunning_ = true;

	AMErrorMon::information(this, AMTESTAPPCONTROLER_STARTED, "Time Evolution Strip Tool started successfully...");

	return true;
}

bool AMTESTAppController::shutdown()
{
	AMErrorMon::information(this, AMTESTAPPCONTROLER_SHUTTING_DOWN, "Time Evolution Strip Tool is now shutting down...");

	AMDSClientAppController::releaseAppController();
	AMTESTStripTool::releaseStripTool();

	AMErrorMon::information(this, AMTESTAPPCONTROLER_SHUTDOWN, "Time Evolution Strip Tool has shutdown...");

	return true;
}

bool AMTESTAppController::isRunning()
{
	return isRunning_;
}

void AMTESTAppController::onClientControllerConnected()
{
	AMErrorMon::information(this, AMTESTAPPCONTROLER_DATASERVER_CONNECTED, "Opened network session with data server...");
}

void AMTESTAppController::registerClasses()
{
	AMErrorMon::information(this, AMTESTAPPCONTROLER_REGISTERING_CLASSES, "Registering necessary classes...");
	AMDSClientRequestSupport::registerClientRequestClass();
	AMDSDataHolderSupport::registerDataHolderClass();
	AMDSEventDataSupport::registerEventDataObjectClass();
}

void AMTESTAppController::initializeDataServer()
{
	AMErrorMon::information(this, AMTESTAPPCONTROLER_INITIALIZING_DATASERVER, "Initializing the data server...");
	AMDSClientAppController *dataServer = AMDSClientAppController::clientAppController();
	connect(dataServer, SIGNAL(AMDSClientControllerConnected()), this, SLOT(onClientControllerConnected()));
}

void AMTESTAppController::setupUI()
{
	AMErrorMon::information(this, AMTESTAPPCONTROLER_SETTING_UP_GUI, "Setting up the GUI...");
	mainWindow_ = new AMTESTMainWindow;

	AMTESTCentralWidgetView *centralWidget = new AMTESTCentralWidgetView;
	mainWindow_->setCentralWidget(centralWidget);

	mainWindow_->show();
}
