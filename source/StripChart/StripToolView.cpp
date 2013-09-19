#include "StripChart/StripTool.h"

StripTool::StripTool(QWidget *parent)
    : QMainWindow(parent)
{
    //  set some basic parameters for how the plot looks. These may be defined by the user later.
    maxPointsDisplayed_ = 10;

    //  create the "active pvs" toolbar widget.
    createPVDock();

    //  create the menu bar items for this application.
    createFileMenu();
    createPlotMenu();
    createViewMenu();

    //  now create the MPlot that will display info for all added pvs.

    //  eventually, I'd like the selector to dictate the axis labels displayed on the plot.
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
    togglePVListAction_ = pvDock_->toggleViewAction();

    //  create the view menu and add its actions!
    viewMenu_ = menuBar()->addMenu("&View");
    viewMenu_->addAction("Plot palette"); //show options for changing plot colors.
    viewMenu_->addAction("Line palette"); //show options for changing line colors, weights, markers, etc.
    viewMenu_->addSeparator();
    viewMenu_->addAction(togglePVListAction_);
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
    //  right now, only interested in the ring current pv!
    //      - it is a single value that updates regularly.
    //      - only want to look at adding one pv for now.
    //  eventually, these restrictions will be lifted.

    if (newPVName == "PCT1402-01:mA:fbk")
    {
        updateNumber_ = 0;
        updateNumbers_ = QVector<double>(0);
        pvValues_ = QVector<double>(0);

        //  connect to the new pv and add it to this plot window.
        AMReadOnlyPVControl *newPV = new AMReadOnlyPVControl(newPVName, newPVName, this, newPVDescription);
        connect( newPV, SIGNAL(connected(bool)), this, SLOT(onNewPVConnected(bool)) );
        connect( newPV, SIGNAL(valueChanged(double)), this, SLOT(onNewPVUpdate(double)) );

        //  add new pv info to the list of currently active pvs.
        addToPVList(newPVName, newPVDescription);

        //  create the necessary objects to add the updating pv values to the plot.
        pvSeriesData_ = new MPlotVectorSeriesData();

        pvSeries_ = new MPlotSeriesBasic();
        pvSeries_->setDescription(newPVDescription);

        pvSeries_->setModel(pvSeriesData_);

        plot->addItem(pvSeries_);
    }
}


void StripTool::onNewPVConnected(bool isConnected)
{
    Q_UNUSED(isConnected);
    //  do something special when the pv is initially connected.
    //  maybe enable the pv list item? List item is disabled when pv is not connected?
}


void StripTool::onNewPVUpdate(double newValue)
{
    //  update the model tracking all of the pv's values.
    updateNumbers_.append(updateNumber_);
    pvValues_.append(newValue);

    //  initialize the vectors containing the x values and the y values to be displayed.
    QVector<double> updateNumbersDisplayed;
    QVector<double> pvValuesDisplayed;

    //  update the values displayed on the plot.
    if (updateNumber_ < maxPointsDisplayed_) {
        updateNumbersDisplayed = updateNumbers_;
        pvValuesDisplayed = pvValues_;

    } else {
        updateNumbersDisplayed = updateNumbers_.mid(updateNumber_ - maxPointsDisplayed_);
        pvValuesDisplayed = pvValues_.mid(updateNumber_ - maxPointsDisplayed_);

    }

    //  update the vector series data.
    pvSeriesData_->setValues(updateNumbersDisplayed, pvValuesDisplayed);

    //  increment the updateNumber to prepare for the next value update.
    updateNumber_++;
}


void StripTool::addToPVList(const QString &newPVName, const QString &newPVDescription)
{
    QListWidgetItem *pvEntry = new QListWidgetItem(newPVDescription);
    pvEntry->setCheckState(Qt::Checked); // pv should be hidden on the graph if it is unchecked.
    pvEntry->setToolTip(newPVName);
//    pvEntry->setFlags(Qt::ItemIsEditable); // I want to be able to edit the pv description after it is added.

    pvList_->addItem(pvEntry);
}


void StripTool::onNewPVCancelled()
{
    //  maybe display confirmation message, or something?
}
