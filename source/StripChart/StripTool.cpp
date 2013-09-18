#include "StripChart/StripTool.h"

StripTool::StripTool(QWidget *parent)
    : QMainWindow(parent)
{
    //  create the menu bar items for this application.
    createFileMenu();
    createPlotMenu();
    createViewMenu();

    //  create the "active pvs" toolbar widget.
    createPVDock();

    //  now create the MPlot that will display info for all added pvs.
    plot = new MPlot();

    MPlotWidget *plotWidget = new MPlotWidget();
    plotWidget->setPlot(plot);

    //  we can add a new pv or close the application using buttons.
    addPVButton_ = new QPushButton();
    addPVButton_->setText("Add PV");
    connect( addPVButton_, SIGNAL(clicked()), addPVAction_, SLOT(trigger()) );

    quitButton_ = new QPushButton();
    quitButton_->setText("Quit");
    connect( quitButton_, SIGNAL(clicked()), quitAction_, SLOT(trigger()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addPVButton_);
    buttonLayout->addWidget(quitButton_);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(plotWidget);
    windowLayout->addLayout(buttonLayout);

    QWidget *windowContents = new QWidget();
    windowContents->setLayout(windowLayout);

    setCentralWidget(windowContents);
    setWindowTitle("Strip Chart Tool");
    resize(700, 500);
}


void StripTool::createFileMenu()
{
    //  create the actions used in the file menu.
    quitAction_ = new QAction("Quit", this);
    connect( quitAction_, SIGNAL(triggered()), qApp, SLOT(quit()) );

    //  create the file menu and add its actions.
    fileMenu_ = menuBar()->addMenu(tr("&File"));
    fileMenu_->addAction("New plot");
    fileMenu_->addSeparator();
    fileMenu_->addAction("Open plot");
    fileMenu_->addSeparator();
    fileMenu_->addAction("Save data");
    fileMenu_->addAction("Save plot");
    fileMenu_->addSeparator();
    fileMenu_->addAction(quitAction_);
}


void StripTool::createPlotMenu()
{
    //  create the actions used in the plot menu.
    addPVAction_ = new QAction("Add PV", this);
    connect( addPVAction_, SIGNAL(triggered()), this, SLOT(onAddPVAction()) );

    //  create the plot menu and add the appropriate actions.
    plotMenu_ = menuBar()->addMenu("&Plot");
    plotMenu_->addAction(addPVAction_);
}


void StripTool::createViewMenu()
{
    //  create the actions used in the plot menu.
    showPVListAction_ = new QAction("Show active PVs", this);
    connect( showPVListAction_, SIGNAL(triggered()), this, SLOT(onShowPVListAction()) );

    hidePVListAction_ = new QAction("Hide active PVs", this);
    connect( showPVListAction_, SIGNAL(triggered()), this, SLOT(onHidePVListAction()) );

    //  but we can still create view menu and populate it with daydreams.
    viewMenu_ = menuBar()->addMenu("&View");
    viewMenu_->addAction("Plot palette"); //show options for changing plot colors.
    viewMenu_->addAction("Line palette"); //show options for changing line colors, weights, markers, etc.
    viewMenu_->addSeparator();
    viewMenu_->addAction(showPVListAction_); //should be disabled initially (eventually).
    viewMenu_->addAction(hidePVListAction_); //should be enabled initially (eventually).
}


void StripTool::createPVDock()
{
    pvList_ = new QListWidget();
//    pvList_->setBaseSize(200, 400);

    QVBoxLayout *pvDockLayout = new QVBoxLayout();
    pvDockLayout->addWidget(pvList_);

    QGroupBox *pvDockGroup = new QGroupBox();
    pvDockGroup->setLayout(pvDockLayout);

    pvDock_ = new QDockWidget("Active PVs", this);
    pvDock_->setAllowedAreas(Qt::RightDockWidgetArea);
    pvDock_->setWidget(pvDockGroup);

    addDockWidget(Qt::RightDockWidgetArea, pvDock_);
}


StripTool::~StripTool()
{
    
}


void StripTool::onAddPVAction()
{
    addPVDialog_ = new AddPVDialog(this);
    connect( addPVDialog_, SIGNAL(newPVAccepted(QString, QString)), this, SLOT(onNewPVAccepted(QString, QString)) );
    connect( addPVDialog_, SIGNAL(rejected()), this, SLOT(onNewPVCancelled()) );
    addPVDialog_->exec();
}


void StripTool::onNewPVAccepted(const QString &newPVName, const QString &newPVDescription)
{

    //  connect to the new pv and add it to this plot window.
    AMReadOnlyPVControl *newPV = new AMReadOnlyPVControl(newPVName, newPVName, this, newPVDescription);
    connect( newPV, SIGNAL(connected(bool)), this, SLOT(onNewPVConnected(bool)) );
    connect( newPV, SIGNAL(valueChanged(double)), this, SLOT(onNewPVUpdate(double)) );

    //  add new pv info to the list of currently active pvs.
    addToPVList(newPVName, newPVDescription);
}


void StripTool::onNewPVConnected(bool isConnected)
{
    Q_UNUSED(isConnected);
    //  do something special when the pv is initially connected?
}


void StripTool::onNewPVUpdate(double newValue)
{
    Q_UNUSED(newValue);
    //  update the model tracking the specific pv?
}

void StripTool::addToPVList(const QString &newPVName, const QString &newPVDescription)
{
    QListWidgetItem *pvEntry = new QListWidgetItem(newPVDescription);
    pvEntry->setWhatsThis(newPVName);
    pvEntry->setCheckState(Qt::Checked);

    pvList_->addItem(pvEntry);
}


void StripTool::onNewPVCancelled()
{
    //  maybe display confirmation message, or something?
}


void StripTool::onShowPVListAction()
{
    //  this will show the toolbar we've created, that lists all pvs added!
    //pvDock_->show();
}


void StripTool::onHidePVListAction()
{
    //  this will hide the pvs toolbar.
    //pvDock_->hide();
}
