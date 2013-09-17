#include "StripChart/StripTool.h"

StripTool::StripTool(QWidget *parent)
    : QMainWindow(parent)
{
    //  create the menu bar items for this application.
    createFileMenu();
    createPlotMenu();
    createViewMenu();

    //  initialize variables that will contain pv names and descriptions.
    activePVList_ = new QList<QPair<QString, QString> >();

    //  begin building window widgets.
    MPlotWidget *plotWidget = new MPlotWidget();

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
    //  add new pv to the plot.

    //  add new pv info to the list of currently active pvs.
    addToActivePVList(newPVInfo);
}

void StripTool::addToActivePVList(const QPair<QString, QString> newPVInfo)
{
    getActivePVList()->append(newPVInfo);
}


QList<QPair<QString, QString> >* StripTool::getActivePVList()
{
    return activePVList_;
}


void StripTool::onNewPVCancelled()
{

}
