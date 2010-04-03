#include "MainWindow.h"

#include "acquaman/AMScanController.h"
#include "acquaman/AMDacqScanController.h"

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
	

        //AMScanController *myScan;
        //myScan = new AMDacqScanController(this);
	
	// setWindowFlags(Qt::FramelessWindowHint);
	// showMaximized();
	// qApp->processEvents();
	// qApp->processEvents();
	
	 // QRect screenGeo = QApplication::desktop()->availableGeometry( QApplication::desktop()->primaryScreen() );
	// dag.adjust(0, 44, 0, -44);
	 // setFixedSize(dag.width(), dag.height());
	// this->setGeometry(0, 44, dag.width(), dag.height());

	setContentsMargins(0, 0, 0, 0);

	////////////////////////////////
	// Central Widgets:
	////////////////////////////////
	
	stackWidget_ = new QStackedWidget();
	// pw_->setGeometry(QRectF(0,0,this->width(),this->height()));	
	
	// Main full-screen widgets:
	connectionSettings_ = new ConnectionSettings(stackWidget_);
	stackWidget_->addWidget(connectionSettings_);
	
	samplePositions_ = new SamplePositions(stackWidget_);
	stackWidget_->addWidget(samplePositions_);
	
	gratingResolution_ = new GratingResolution(stackWidget_);
	stackWidget_->addWidget(gratingResolution_);
	
	absorptionScanController_ = new AbsorptionScanController(stackWidget_);
	stackWidget_->addWidget(absorptionScanController_);

	sxscViewer_ = new SGMXASScanConfigurationViewer(stackWidget_);
	stackWidget_->addWidget(sxscViewer_);
	
	emissionScanController_ = new EmissionScanController(stackWidget_);
	stackWidget_->addWidget(emissionScanController_);
	
	scheduler_ = new Scheduler(stackWidget_);
	stackWidget_->addWidget(scheduler_);
	
	periodicTable_ = new PeriodicTable(stackWidget_);
	stackWidget_->addWidget(periodicTable_);
	
	protocolViewer_ = new ProtocolViewer(stackWidget_);
	stackWidget_->addWidget(protocolViewer_);
	
	expAlbum_ = new ExpAlbum(stackWidget_);
	stackWidget_->addWidget(expAlbum_);
	

	////////////////////////////////
	// Left Sidebar: choosing widgets
	////////////////////////////////
	
	sidebar_ = new Sidebar();
	
	sidebar_->addCategory("Beamline Control");
	sidebar_->addLink("Connection Settings", connectionSettings_, "Beamline Control", -1, QIcon(":/network-workgroup.png"));
	sidebar_->addLink("Sample Positions", samplePositions_, "Beamline Control", -1, QIcon(":/system-software-update.png"));
	sidebar_->addLink("Gratings and Resolution", gratingResolution_, "Beamline Control", -1, QIcon(":/system-search.png"));
	
	sidebar_->addCategory("Scan Setup");
	sidebar_->addLink("Absorption Scan", absorptionScanController_, "Scan Setup", -1, QIcon(":/utilities-system-monitor.png"));
	sidebar_->addLink("Emission Scan", emissionScanController_, "Scan Setup", -1, QIcon(":/multimedia-volume-control.png"));
	sidebar_->addLink("David Scan", sxscViewer_, "Scan Setup", -1, QIcon(":/utilities-system-monitor.png"));

	sidebar_->addCategory("Experiment Tools");
	sidebar_->addLink("Scheduler (Brew)", scheduler_, "Experiment Tools", -1, QIcon(":/user-away.png"));
	sidebar_->addLink("Periodic Table", periodicTable_, "Experiment Tools", -1, QIcon(":/applications-science.png"));
	sidebar_->addLink("Protocol", protocolViewer_, "Experiment Tools", -1, QIcon(":/accessories-text-editor.png"));
	sidebar_->addLink("Experiment Album", expAlbum_, "Experiment Tools", -1, QIcon(":/system-file-manager.png"));
	
	sidebar_->addCategory("Now Playing...");
	
	connect(sidebar_, SIGNAL(linkActivated(QWidget*)), stackWidget_, SLOT(setCurrentWidget(QWidget*)));
	
	////////////////////////////////
	// Bottom bar: start, stop, progress bar, etc.
	////////////////////////////////
	
	bottomBar_ = new BottomBar();

        connect(bottomBar_, SIGNAL(pauseScanIssued()), connectionSettings_, SLOT(onStartScan()) );
        connect(bottomBar_, SIGNAL(stopScanIssued()), connectionSettings_, SLOT(onStopScan()) );
		connect(connectionSettings_, SIGNAL(scanProgress(double,double)), bottomBar_, SLOT(updateScanProgress(double,double)));

	// make connections for bottomBar...
	
	////////////////////////////////
	// Build layouts:
	////////////////////////////////
	
	QVBoxLayout* vl1 = new QVBoxLayout(this);
	vl1->setContentsMargins(0, 0, 0, 0);
	vl1->setSpacing(0);
	QHBoxLayout* hl1 = new QHBoxLayout();
	hl1->setContentsMargins(0, 0, 0, 0);
	hl1->setSpacing(0);
	vl1->addLayout(hl1);
	
	
	hl1->addWidget(sidebar_);
	hl1->addWidget(stackWidget_);
	vl1->addWidget(bottomBar_);
	
	
	
	
}
