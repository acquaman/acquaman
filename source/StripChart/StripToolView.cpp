#include "StripChart/StripToolView.h"

StripToolView::StripToolView(QWidget *parent)
    : QMainWindow(parent)
{    
    model = new StripTool(this);

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


void StripToolView::createFileMenu()
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


void StripToolView::createPlotMenu()
{
    //  create the actions used in the plot menu.
    addPVAction_ = new QAction("Add PV", this);
    connect( addPVAction_, SIGNAL(triggered()), this, SLOT(onAddPVAction()) );

    //  create the plot menu and add the appropriate actions.
    plotMenu_ = menuBar()->addMenu("&Plot");
    plotMenu_->addAction(addPVAction_);
}


void StripToolView::createViewMenu()
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


void StripToolView::createPVDock()
{
//    pvList_ = new QListWidget();
//    pvList_->setEditTriggers(QAbstractItemView::DoubleClicked);
//    connect( pvList_, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(toTogglePVVisibility(QListWidgetItem *)) );
//    connect( pvList_, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(toEditPVDescription(QListWidgetItem *)) );

    pvListView_ = new QListView(this);
    pvListView_->setModel(model->getActivePVNames());

    QVBoxLayout *pvDockLayout = new QVBoxLayout();
    //pvDockLayout->addWidget(pvList_);
    pvDockLayout->addWidget(pvListView_);

    QGroupBox *pvDockGroup = new QGroupBox();
    pvDockGroup->setLayout(pvDockLayout);

    pvDock_ = new QDockWidget("Active PVs", this);
    pvDock_->setAllowedAreas(Qt::RightDockWidgetArea);
    pvDock_->setWidget(pvDockGroup);


    addDockWidget(Qt::RightDockWidgetArea, pvDock_);
}


StripToolView::~StripToolView()
{
    
}


void StripToolView::onAddPVAction()
{
    addPVDialog_ = new AddPVDialog(this);
    connect( addPVDialog_, SIGNAL(newPVAccepted(QString, QString)), this, SLOT(onNewPVAccepted(QString, QString)) );
    connect( addPVDialog_, SIGNAL(rejected()), this, SLOT(onNewPVCancelled()) );
    addPVDialog_->exec();
}


void StripToolView::onNewPVAccepted(const QString &newPVName, const QString &newPVDescription)
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

        //plot->addItem(pvSeries_);
    }
}


void StripToolView::onNewPVConnected(bool isConnected)
{
    Q_UNUSED(isConnected);
    //  do something special when the pv is initially connected.
    //  maybe enable the pv list item? List item is disabled when pv is not connected?
}


void StripToolView::onNewPVUpdate(double newValue)
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

    //  increment the updateNumber to prepare for the next single value update.
    updateNumber_++;
}


void StripToolView::addToPVList(const QString &newPVName, const QString &newPVDescription)
{
    QListWidgetItem *pvEntry = new QListWidgetItem(newPVDescription);
    pvEntry->setCheckState(Qt::Checked); // pv should be hidden on the graph if it is unchecked.
    pvEntry->setToolTip(newPVName);
    pvEntry->setFlags(pvEntry->flags() | Qt::ItemIsEditable); // pv description can be edited after it is added.

    model->addToActivePVs(newPVName, newPVDescription);
    //pvList_->addItem(pvEntry);
}


void StripToolView::toTogglePVVisibility(const QListWidgetItem &itemClicked)
{
    Qt::CheckState itemCheckState = itemClicked.checkState();

    if (itemCheckState == Qt::Unchecked)
        emit hidePV(itemClicked);
    else
        emit showPV(itemClicked);
}


void StripToolView::onNewPVCancelled()
{
    //  maybe display confirmation message, or something?
}


void StripToolView::toEditPVDescription(const QListWidgetItem &itemDoubleClicked)
{
    Q_UNUSED(itemDoubleClicked);
}
