#include "StripChart/StripTool.h"

StripTool::StripTool(QWidget *parent)
    : QMainWindow(parent)
{        
    //  set some basic parameters for how the plot looks. These may be defined by the user later.
    maxPointsDisplayed_ = 10;

    //  set up the model for how pv information is stored and accessed, and the view.
    createPVListModel();
    createPVDock();

    //  set up the model containing the pv data as it updates.
    pvDataModel_ = new QStandardItemModel(this);

    //  create the menu bar items for this application.
    createFileMenu();
    createPlotMenu();
    createViewMenu();

    //  now create the MPlot that will display info for all added pvs.
    //  eventually, I'd like the selector to dictate the axis labels/units displayed on the plot.
    MPlotPlotSelectorTool *selector = new MPlotPlotSelectorTool();

    plot = new MPlot();
    plot->addTool(selector);

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



StripTool::~StripTool()
{

}



void StripTool::createPVListModel()
{
    pvListModel_ = new QStandardItemModel(this);
    connect( pvListModel_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(toTogglePVVisibility(QStandardItem*)) );

    pvListView_ = new QListView(this);
    pvListView_->setModel(pvListModel_);

//    pvTableView_ = new QTableView(this);
//    pvTableView_->setModel(pvListModel_);
//    pvTableView_->setShowGrid(false);
}



void StripTool::createFileMenu()
{
    //  create the actions used in the file menu.
    newPlotAction_ = new QAction("New plot", this);
    newPlotAction_->setEnabled(false);

    openPlotAction_ = new QAction("Open plot", this);
    openPlotAction_->setEnabled(false);

    saveDataAction_ = new QAction("Save data", this);
    saveDataAction_->setEnabled(false);

    savePlotAction_ = new QAction("Save plot", this);
    savePlotAction_->setEnabled(false);

    quitAction_ = new QAction("Quit", this);
    connect( quitAction_, SIGNAL(triggered()), qApp, SLOT(quit()) );

    //  create the file menu and add its actions.
    fileMenu_ = menuBar()->addMenu(tr("&File"));
    fileMenu_->addAction(newPlotAction_);
    fileMenu_->addSeparator();
    fileMenu_->addAction(openPlotAction_);
    fileMenu_->addSeparator();
    fileMenu_->addAction(saveDataAction_);
    fileMenu_->addAction(savePlotAction_);
    fileMenu_->addSeparator();
    fileMenu_->addAction(quitAction_);
}



void StripTool::createPlotMenu()
{
    //  create the actions used in the plot menu.
    addPVAction_ = new QAction("Add PV", this);
    connect( addPVAction_, SIGNAL(triggered()), this, SLOT(onAddPVAction()) );

    removePVAction_ = new QAction("Remove PV", this);
    removePVAction_->setEnabled(false);

    //  create the plot menu and add the appropriate actions.
    plotMenu_ = menuBar()->addMenu("&Plot");
    plotMenu_->addAction(addPVAction_);
    plotMenu_->addAction("Remove PV");
}



void StripTool::createViewMenu()
{
    //  create the actions used in the plot menu.
    togglePlotPaletteAction_ = new QAction("Plot palette", this);
    togglePlotPaletteAction_->setEnabled(false);

    toggleLinePaletteAction_ = new QAction("Line palette", this);
    toggleLinePaletteAction_->setEnabled(false);

    togglePVListAction_ = pvDock_->toggleViewAction();

    //  create the view menu and add its actions!
    viewMenu_ = menuBar()->addMenu("&View");
    viewMenu_->addAction(togglePlotPaletteAction_); //show options for changing plot colors.
    viewMenu_->addAction(toggleLinePaletteAction_); //show options for changing line colors, weights, markers, etc.
    viewMenu_->addSeparator();
    viewMenu_->addAction(togglePVListAction_);
}



void StripTool::createPVDock()
{
    QVBoxLayout *pvDockLayout = new QVBoxLayout();
    pvDockLayout->addWidget(pvListView_);
//    pvDockLayout->addWidget(pvTableView_);

    QGroupBox *pvDockGroup = new QGroupBox();
    pvDockGroup->setLayout(pvDockLayout);

    pvDock_ = new QDockWidget("Process Variables", this);
    pvDock_->setAllowedAreas(Qt::RightDockWidgetArea);
    pvDock_->setWidget(pvDockGroup);

    addDockWidget(Qt::RightDockWidgetArea, pvDock_);
}



void StripTool::onAddPVAction()
{
    addPVDialog_ = new AddPVDialog(this);
    connect( addPVDialog_, SIGNAL(newPVAccepted(QString, QString)), this, SLOT(addToPVListModel(QString, QString)) );
    addPVDialog_->exec();
}



void StripTool::addToPVListModel(const QString &newPVName, const QString &newPVDescription)
{
    QStandardItem *newPVEntry = new QStandardItem(newPVName);
    newPVEntry->setCheckable(true);
    newPVEntry->setCheckState(Qt::Checked);

    pvListModel_->appendRow(newPVEntry);

    AMReadOnlyPVControl *newControl = new AMReadOnlyPVControl(newPVName, newPVName, this);
    connect( newControl, SIGNAL(valueChanged(double)), this, SLOT(onPVValueChanged(double)) );

    StripToolItem *item = new StripToolItem(newPVName, newPVDescription, this);

    itemList_.append(item);
}



void StripTool::onPVValueChanged(double newValue)
{
    Q_UNUSED(newValue);
}



void StripTool::toTogglePVVisibility(QStandardItem *changedItem)
{
    pvListModel_->indexFromItem(changedItem);
}




