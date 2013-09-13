#include "striptool.h"

StripTool::StripTool(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createMenus();

    activePVList = new QList<QString>();

    MPlotWidget *plotWidget = new MPlotWidget();

    addPVButton = new QPushButton();
    addPVButton->setText("Add PV");
    //connect( addPVButton, SIGNAL(clicked()), addPVAction, SLOT(trigger()) );

    quitButton = new QPushButton();
    quitButton->setText("Quit");
    //connect( quitButton, SIGNAL(clicked()), quitAction, SLOT(trigger()) );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addPVButton);
    buttonLayout->addWidget(quitButton);

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(plotWidget);
    windowLayout->addLayout(buttonLayout);

    QWidget *windowContents = new QWidget();
    windowContents->setLayout(windowLayout);

    setCentralWidget(windowContents);
    setWindowTitle("Strip Chart Tool");

}


void StripTool::createActions()
{
    quitAction_ = new QAction("Quit", this);
    connect( quitAction_, SIGNAL(triggered()), qApp, SLOT(quit()) );

    addPVAction_ = new QAction("Add PV", this);
    connect( addPVAction_, SIGNAL(triggered()), this, SLOT(onAddPVAction()) );
}


void StripTool::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    //fileMenu->addAction(quitAction);
    fileMenu->addSeparator();
    fileMenu->addAction("New plot");
//    fileMenu->addAction("New PV");
    fileMenu->addSeparator();
    fileMenu->addAction("Save data");
    fileMenu->addAction("Save plot");
    fileMenu->addSeparator();
    fileMenu->addAction("Quit");
//    fileMenu->addAction("Quit that!");
//    fileMenu->addAction("Escape that!");
//    fileMenu->addAction("Close");

    plotMenu = menuBar()->addMenu("&Plot");
//    plotMenu->addAction(addPVAction);

    viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("Plot palette");
    viewMenu->addAction("Line palette");
    viewMenu->addSeparator();
    viewMenu->addAction("Show active PVs");
    viewMenu->addAction("Hide active PVs");
}


StripTool::~StripTool()
{
    
}


QList<QString>* StripTool::getActivePVList()
{
    return activePVList;
}


void StripTool::addToActivePVList(const QString newPVName)
{
    getActivePVList()->append(newPVName);
}
