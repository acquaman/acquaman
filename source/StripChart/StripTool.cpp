#include "StripChart/StripTool.h"

StripTool::StripTool(QWidget *parent)
    : QMainWindow(parent)
{
    //  create the menu bar items for this application.
    createFileMenu();
    createPlotMenu();
    createViewMenu();

    //  begin building window widgets.
    //  initialize variables that will contain pv names and descriptions.
    pvsAdded = new QLabel("PVs added : ");
    activePVList_ = new QList<QPair<QString, QString> >();
    activePVListView_ = new QListWidget(this);
//    activePVListView_->setModel(activePVList_);

    QHBoxLayout *pvsAddedLayout = new QHBoxLayout();
    pvsAddedLayout->addWidget(pvsAdded);
    pvsAddedLayout->addWidget(activePVListView_);

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
    windowLayout->addLayout(pvsAddedLayout);
    windowLayout->addWidget(plotWidget);
    windowLayout->addLayout(buttonLayout);

    QWidget *windowContents = new QWidget();
    windowContents->setLayout(windowLayout);

    setCentralWidget(windowContents);
    setWindowTitle("Strip Chart Tool");
    resize(500, 500);
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
    //  no view actions defined yet!

    //  but we can still create view menu and populate it with daydreams.
    viewMenu_ = menuBar()->addMenu("&View");
    viewMenu_->addAction("Plot palette");
    viewMenu_->addAction("Line palette");
    viewMenu_->addSeparator();
    viewMenu_->addAction("Show active PVs");
    viewMenu_->addAction("Hide active PVs");
}


StripTool::~StripTool()
{
    
}


void StripTool::onAddPVAction()
{
    addPVDialog_ = new AddPVDialog(this);
    connect( addPVDialog_, SIGNAL(newPVAccepted(QPair<QString, QString>)), this, SLOT(onNewPVAccepted(QPair<QString, QString>)) );
    connect( addPVDialog_, SIGNAL(rejected()), this, SLOT(onNewPVCancelled()) );

    addPVDialog_->exec();
}


void StripTool::onNewPVAccepted(const QPair<QString, QString> newPVInfo)
{

    //  connect to the new pv and add it to this plot window.
    //  think about maybe using a separate class to handle connecting to PVs and managing their properties?
    if (newPVInfo.first == "PCT1402-01:mA:fbk")
    {
        AMReadOnlyPVControl *newPV = new AMReadOnlyPVControl(newPVInfo.first, newPVInfo.first, this, newPVInfo.second);
        connect( newPV, SIGNAL(connected(bool)), this, SLOT(onNewPVConnected(bool)) );
        connect( newPV, SIGNAL(valueChanged(double)), this, SLOT(onNewPVUpdate(double)) );

        //  add new pv info to the list of currently active pvs.
        addToActivePVList(newPVInfo);
    }
}


void StripTool::onNewPVConnected(bool isConnected)
{
    Q_UNUSED(isConnected);
}


void StripTool::onNewPVUpdate(double newValue)
{
    Q_UNUSED(newValue);
}

void StripTool::addToActivePVList(const QPair<QString, QString> newPVInfo)
{
    getActivePVList()->append(newPVInfo);
}


QList<QPair<QString, QString> >* StripTool::getActivePVList()
{
    return activePVList_;
}

int StripTool::activePVCount()
{
    return activePVList_->count();
}


void StripTool::onNewPVCancelled()
{

}
