#include "StripChart/StripTool.h"

StripTool::StripTool(QWidget *parent)
    : QMainWindow(parent)
{
    //  set up the model for how pv information is stored and accessed, and the view.
    createPVListModel();
    createPVDock();

    //  create the menu bar items for this application.
    createFileMenu();
    createPlotMenu();
    createViewMenu();
    createSettingsMenu();

    //  now create the MPlot that will display info for all added pvs.
    MPlotPlotSelectorTool *selector = new MPlotPlotSelectorTool();
    connect( selector, SIGNAL(itemSelected(MPlotItem*)), this, SLOT(showItemInfo(MPlotItem*)) );

    plot_ = new MPlot();
    plot_->addTool(selector);

    MPlotWidget *plotWidget = new MPlotWidget();
    plotWidget->setPlot(plot_);

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
    connect(pvListModel_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(togglePVVisibility(QStandardItem*)) );

    pvListView_ = new QListView(this);
    pvListView_->setModel(pvListModel_);
    pvListView_->setEditTriggers(QAbstractItemView::SelectedClicked);

    itemContainer = new StripToolContainer(this);
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

    addSR1CurrentAction_ = new QAction("Add SR1 Current", this);
    connect( addSR1CurrentAction_, SIGNAL(triggered()), this, SLOT(onAddSR1CurrentAction()) );

    addPVGroupAction_ = new QAction("Add PV Group", this);
    addPVGroupAction_->setEnabled(false);
    connect( addPVGroupAction_, SIGNAL(triggered()), this, SLOT(onAddPVGroupAction()) );

    //  create the plot menu and add the appropriate actions.
    plotMenu_ = menuBar()->addMenu("&Plot");
    plotMenu_->addAction(addPVAction_);
    plotMenu_->addAction(removePVAction_);
    plotMenu_->addSeparator();
    plotMenu_->addAction(addSR1CurrentAction_);
    plotMenu_->addSeparator();
    plotMenu_->addAction(addPVGroupAction_);
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



void StripTool::createSettingsMenu()
{
    setPVGroupAction_ = new QAction("Set PV Group", this);
    connect( setPVGroupAction_, SIGNAL(triggered()), this, SLOT(onSetPVGroupAction()) );

    settingsMenu_ = menuBar()->addMenu("&Settings");
    settingsMenu_->addAction(setPVGroupAction_);

}



void StripTool::createPVDock()
{
    QVBoxLayout *pvDockLayout = new QVBoxLayout();
    pvDockLayout->addWidget(pvListView_);

    QGroupBox *pvDockGroup = new QGroupBox();
    pvDockGroup->setLayout(pvDockLayout);

    pvDock_ = new QDockWidget("Process Variable Tools", this);
    pvDock_->setAllowedAreas(Qt::RightDockWidgetArea);
    pvDock_->setWidget(pvDockGroup);

    addDockWidget(Qt::RightDockWidgetArea, pvDock_);
}



void StripTool::onAddPVAction()
{
    if (addPVDialog_.exec() == QDialog::Accepted)
        addToPVListModel(addPVDialog_.pvName(), addPVDialog_.pvDescription(), addPVDialog_.pvUnits());
}



void StripTool::onAddSR1CurrentAction()
{
    addToPVListModel("PCT1402-01:mA:fbk", "SR1 Current", "mA");
}



void StripTool::onAddPVGroupAction()
{
    QList<QList<QString> > *pvGroupInfo = new QList<QList<QString> >();

    //  look for a file containing the pv names, descriptions, and units for the pv group, and load them automatically!
    QString filename = QFileDialog::getOpenFileName(this, "Open pv group", "", "PV Group (*.txt);;All Files (*)");
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly))
    {
        QDataStream in(&file);
        in.setVersion(QDataStream ::Qt_4_5);
        in >> *pvGroupInfo;

    } else {
        QMessageBox::information(this, "Unable to open file", file.errorString());
    }

    while(!pvGroupInfo->isEmpty())
    {
        QList<QString> newPVInfo = pvGroupInfo->takeFirst();
        QString pvName = newPVInfo.at(0);
        QString pvDescription = newPVInfo.at(1);
        QString pvUnits = newPVInfo.at(2);

        addToPVListModel(pvName, pvDescription, pvUnits);
    }
}



QList<QList<QString> > StripTool::getActivePVList()
{
    //  go through the model containing currently active pvs and select all pvs that are checked.
    //QList<QStandardItem *> pvGroup = pvListModel_->findItems("*", Qt::CheckState = Qt::Checked);

    //  having trouble figuring that out. Temporary solution is to keep lists of visible/all pv names.
    int count = visiblePVNames_.size();
    QList<QList<QString> > pvGroupInfo_;
    pvGroupInfo_.reserve(count);

    QList<QString> pvInfo_;
    pvInfo_.reserve(3);

    for (int i = 0; i < count; i++) {
        QString pvName = visiblePVNames_.at(i);
        pvInfo_.append(pvName);
        pvInfo_.append(itemContainer->description(pvName));
        pvInfo_.append(itemContainer->yUnits(pvName));

        pvGroupInfo_.append(pvInfo_);
    }

    return pvGroupInfo_;
}



void StripTool::onSetPVGroupAction()
{
    QList<QList<QString> > pvsToSave = getActivePVList();

    //  open a file to save the pv group in.
    QString filename = QFileDialog::getSaveFileName(this, "Save pv group", "/Users/helfrij/dev/acquaman/source/StripChart/", "PV Group (*.txt);;All Files (*)");
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out.setVersion(QDataStream ::Qt_4_5);
        out << pvsToSave;

    } else {

        QMessageBox::information(this, "Unable to open file", file.errorString());
    }


}



void StripTool::addToPVListModel(const QString &newPVName, const QString &newPVDescription, const QString &newPVUnits)
{
    if (allPVNames_.contains(newPVName))
    {
        QMessageBox::information(this, "Unable to add pv", "A PV with that name is already plotted!");
        //  if the pv has already been added and it is unchecked, maybe make it checked?

    } else {

        QStandardItem *newPVEntry = new QStandardItem();
        if (newPVDescription == "")
            newPVEntry->setText(newPVName);
        else
            newPVEntry->setText(newPVDescription);

        newPVEntry->setColumnCount(1);
        newPVEntry->setChild(0, new QStandardItem(newPVName));

        newPVEntry->setCheckable(true);
        newPVEntry->setCheckState(Qt::Checked);
        newPVEntry->setToolTip(newPVName);
        newPVEntry->setEditable(true);

        pvListModel_->appendRow(newPVEntry);

        itemContainer->addItem(newPVName, newPVDescription, newPVUnits);

        addPVToPlot(newPVName);
    }
}


void StripTool::addPVToPlot(const QString &pvName)
{
    if (itemContainer->contains(pvName))
    {
        plot_->addItem(itemContainer->series(pvName));
        visiblePVNames_.append(pvName);
        allPVNames_.append(pvName);
    }
}


void StripTool::removePVFromPlot(const QString &pvName)
{
    //  remove the pv's instances in the plot and visible pvs list.
    if (itemContainer->contains(pvName))
    {
        plot_->removeItem(itemContainer->series(pvName));
        visiblePVNames_.removeAll(pvName);
    }
}


void StripTool::deletePV(const QString &pvName)
{    
    // remove the pv from the plot, pvListModel, itemContainer, all pvs, and visible pvs.
    if (itemContainer->contains(pvName))
    {
        //  also, delete from list model.
        itemContainer->deleteItem(pvName);
        removePVFromPlot(pvName);
        allPVNames_.removeAll(pvName);
    }
}


void StripTool::togglePVVisibility(QStandardItem *entryChanged)
{
    //  access the entry's pv name, and remove it from the plot and list of visible pvs.
    int checkState = entryChanged->checkState();
    QString pvName = entryChanged->child(0, 0)->text();

    if (checkState == Qt::Checked)
        addPVToPlot(pvName);
    else if (checkState == Qt::Unchecked)
        removePVFromPlot(pvName);
}


void StripTool::showItemInfo(MPlotItem* plotSelection)
{

    updatePlotAxesLabels(plotSelection);
}


void StripTool::updatePlotAxesLabels(MPlotItem *plotSelection)
{
    QString axisLeftLabel = itemContainer->getAxisLeft(plotSelection);
    plot_->axisLeft()->setAxisName(axisLeftLabel);
    plot_->axisLeft()->showAxisName(true);

    QString axisBottomLabel = itemContainer->getAxisBottom(plotSelection);
    plot_->axisBottom()->setAxisName(axisBottomLabel);
    plot_->axisBottom()->showAxisName(true);
}



