#include "StripChart/StripTool.h"

StripTool::StripTool(QWidget *parent) : QMainWindow(parent)
{
    createActions();

    pvListModel_ = new StripToolPVListModel(this);
    connect(pvListModel_, SIGNAL(showCheckedPV(QString)), this, SLOT(onShowCheckedPV(QString)) );
    connect(pvListModel_, SIGNAL(removeUncheckedPV(QString)), this, SLOT(onRemoveUncheckedPV(QString)) );

    pvListView_ = new QListView(this);
    pvListView_->setModel(pvListModel_->model());
    pvListView_->setEditTriggers(QAbstractItemView::SelectedClicked);
    pvListView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

    pvListView_->setContextMenuPolicy(Qt::ActionsContextMenu);
    pvListView_->addAction(pausePVAction_);
    pvListView_->addAction(restartPVAction_);
    pvListView_->addAction(deletePVAction_);

    createPVDock();

    itemContainer = new StripToolContainer(this);

    plot_ = new StripToolPlot(this);

    MPlotWidget *plotWidget = new MPlotWidget();
    plotWidget->setPlot(plot_->plot());

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(plotWidget);

    QWidget *windowContents = new QWidget();
    windowContents->setLayout(windowLayout);

    setCentralWidget(windowContents);
    setWindowTitle("Strip Chart Tool");
    resize(700, 500);

    createMenus();
}



StripTool::~StripTool()
{
}


void StripTool::createActions()
{
    newPlotAction_ = new QAction("New plot", this);
    newPlotAction_->setEnabled(false);

    saveDataAction_ = new QAction("Save data", this);
    saveDataAction_->setEnabled(false);

    quitAction_ = new QAction("Quit", this);
    connect( quitAction_, SIGNAL(triggered()), qApp, SLOT(quit()) );

    addPVAction_ = new QAction("Add PV", this);
    connect( addPVAction_, SIGNAL(triggered()), this, SLOT(onAddPVAction()) );

    addSR1CurrentAction_ = new QAction("Add SR1 Current", this);
    connect( addSR1CurrentAction_, SIGNAL(triggered()), this, SLOT(onAddSR1CurrentAction()) );

    addPVGroupAction_ = new QAction("Add PV Group", this);
    addPVGroupAction_->setEnabled(false);
    connect( addPVGroupAction_, SIGNAL(triggered()), this, SLOT(onAddPVGroupAction()) );

    pausePVAction_ = new QAction("Pause PV", this);
    connect( pausePVAction_, SIGNAL(triggered()), this, SLOT(onPausePVAction()) );

    restartPVAction_ = new QAction("Restart PV", this);
    connect( restartPVAction_, SIGNAL(triggered()), this, SLOT(onRestartPVAction()) );

    removePVAction_ = new QAction("Remove PV", this);
    connect( removePVAction_, SIGNAL(triggered()), this, SLOT(onRemovePVAction()) );

    deletePVAction_ = new QAction("Delete PV", this);
    connect( deletePVAction_, SIGNAL(triggered()), this, SLOT(onDeletePVAction()) );

    savePVGroupAction_ = new QAction("Save PV Group", this);
    connect( savePVGroupAction_, SIGNAL(triggered()), this, SLOT(onSavePVGroupAction()) );

    showLessPVAction_ = new QAction("Show Less", this);
    showLessPVAction_->setEnabled(false);
    connect( showLessPVAction_, SIGNAL(triggered()), this, SLOT(onShowLessPVAction()) );

    showMorePVAction_ = new QAction("Show More", this);
    showMorePVAction_->setEnabled(false);
    connect( showMorePVAction_, SIGNAL(triggered()), this, SLOT(onShowMorePVAction()) );
}



void StripTool::createMenus()
{
    //  create the file menu and add its actions.
    fileMenu_ = menuBar()->addMenu(tr("&File"));
    fileMenu_->addAction(newPlotAction_);
    fileMenu_->addSeparator();
    fileMenu_->addAction(savePVGroupAction_);
    fileMenu_->addAction(saveDataAction_);
    fileMenu_->addSeparator();
    fileMenu_->addAction(quitAction_);

    //  create the plot menu and add the appropriate actions.
    plotMenu_ = menuBar()->addMenu("&Plot");
    plotMenu_->addAction(addPVAction_);
    plotMenu_->addAction(addSR1CurrentAction_);
    plotMenu_->addAction(addPVGroupAction_);
    plotMenu_->addSeparator();
    plotMenu_->addAction(pausePVAction_);
    plotMenu_->addAction(restartPVAction_);
    plotMenu_->addSeparator();
    plotMenu_->addAction(removePVAction_);
    plotMenu_->addAction(deletePVAction_);

    //  create the view menu and add its actions!
    viewMenu_ = menuBar()->addMenu("&View");
    viewMenu_->addAction(toggleDockAction_);
}



void StripTool::createPVDock()
{    
    QSizePolicy *dockSizePolicy = new QSizePolicy();
    dockSizePolicy->setHorizontalPolicy(QSizePolicy::Minimum);

    QVBoxLayout *listViewLayout = new QVBoxLayout();
    listViewLayout->addWidget(pvListView_);

    QGroupBox *listViewBox = new QGroupBox();
    listViewBox->setLayout(listViewLayout);
    listViewBox->setFlat(true);

    QPushButton *addPVButton = new QPushButton("Add PV");
    connect( addPVButton, SIGNAL(clicked()), addPVAction_, SLOT(trigger()) );

    QPushButton *pausePVButton = new QPushButton("Pause PV");
    connect( pausePVButton, SIGNAL(clicked()), pausePVAction_, SLOT(trigger()) );

    QPushButton *restartPVButton = new QPushButton("Restart PV");
    connect( restartPVButton, SIGNAL(clicked()), restartPVAction_, SLOT(trigger()) );

    QPushButton *deletePVButton = new QPushButton("Delete PV");
    connect( deletePVButton, SIGNAL(clicked()), deletePVAction_, SLOT(trigger()) );

    QGridLayout *pvButtonLayout = new QGridLayout();
    pvButtonLayout->addWidget(addPVButton, 0, 0);
    pvButtonLayout->addWidget(deletePVButton, 1, 0);
    pvButtonLayout->addWidget(pausePVButton, 0, 1);
    pvButtonLayout->addWidget(restartPVButton, 1, 1);

    QGroupBox *buttonBox = new QGroupBox();
    buttonBox->setLayout(pvButtonLayout);
    buttonBox->setFlat(true);

    QPushButton *showLessButton = new QPushButton("Less");
    connect( showLessButton, SIGNAL(clicked()), showLessPVAction_, SLOT(trigger()) );

    QPushButton *showMoreButton = new QPushButton("More");
    connect( showMoreButton, SIGNAL(clicked()), showMorePVAction_, SLOT(trigger()) );

    QHBoxLayout *showButtonLayout = new QHBoxLayout();
    showButtonLayout->addWidget(showLessButton);
    showButtonLayout->addWidget(showMoreButton);

    QGroupBox *showBox = new QGroupBox();
    showBox->setLayout(showButtonLayout);
    showBox->setFlat(true);

    QVBoxLayout *pvDockLayout = new QVBoxLayout();
    pvDockLayout->addWidget(listViewBox);
    pvDockLayout->addWidget(buttonBox);
    pvDockLayout->addWidget(showBox);

    QGroupBox *pvDockGroup = new QGroupBox();
    pvDockGroup->setLayout(pvDockLayout);

    pvDock_ = new QDockWidget("Process Variable Tools", this);
    pvDock_->setAllowedAreas(Qt::RightDockWidgetArea);
    pvDock_->setWidget(pvDockGroup);

    toggleDockAction_ = pvDock_->toggleViewAction();

    addDockWidget(Qt::RightDockWidgetArea, pvDock_);
}



void StripTool::onAddPVAction()
{
    if (addPVDialog_.exec() == QDialog::Accepted)
        addNewPV(addPVDialog_.pvName(), addPVDialog_.pvDescription(), addPVDialog_.pvUnits());
}



void StripTool::onAddSR1CurrentAction()
{
    addNewPV("PCT1402-01:mA:fbk", "SR1 Current", "mA");
}



void StripTool::onAddPVGroupAction()
{
    QList<QList<QString> > *pvGroupInfo = new QList<QList<QString> >();

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

        addNewPV(pvName, pvDescription, pvUnits);
    }
}



void StripTool::onPausePVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        QString pvName = pvListModel_->pvNameFromIndex(index);
        itemContainer->setPVUpdating(pvName, false);
    }
}



void StripTool::onRestartPVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        QString pvName = pvListModel_->pvNameFromIndex(index);
        itemContainer->setPVUpdating(pvName, true);
    }
}



void StripTool::onRemovePVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        plot_->hideSeries(pvListModel_->pvNameFromIndex(index));
    }
}



void StripTool::onDeletePVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        deletePV(pvListModel_->pvNameFromIndex(index), index);
    }
}



void StripTool::onShowLessPVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        QString pvName = pvListModel_->pvNameFromIndex(index);
        int showPVAmount = itemContainer->valuesDisplayed(pvName);

        if (showPVAmount > 5)
            showPVAmount -= 5;
        else
            showPVAmount = 5;

        itemContainer->setValuesDisplayed(pvName, showPVAmount);
    }
}



void StripTool::onShowMorePVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        QString pvName = pvListModel_->pvNameFromIndex(index);
        int showPVAmount = itemContainer->valuesDisplayed(pvName);
        showPVAmount += 5;
        itemContainer->setValuesDisplayed(pvName, showPVAmount);
    }
}



void StripTool::onSavePVGroupAction()
{
    QList<QStringList> allPVInfo;

    QList<QString> pvNames = plot_->getActivePVList();
    foreach (const QString &pvName, pvNames)
    {
        QString pvDescription = itemContainer->description(pvName);
        QString pvUnit = itemContainer->yUnits(pvName);

        QStringList pvInfo;
        pvInfo << pvName << pvDescription << pvUnit;
        allPVInfo << pvInfo;
    }

    //  open a file to save the pv group in.
    QString filename = QFileDialog::getSaveFileName(this, "Save pv group", "/Users/helfrij/dev/acquaman/source/StripChart/", "PV Group (*.txt);;All Files (*)");
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        out.setVersion(QDataStream ::Qt_4_5);
        out << allPVInfo;

    } else {

        QMessageBox::information(this, "Unable to open file", file.errorString());
    }
}



void StripTool::addNewPV(const QString &newPVName, const QString &newPVDescription, const QString &newPVUnits)
{
    pvListModel_->addPV(newPVName, newPVDescription, newPVUnits);
    itemContainer->addItem(newPVName, newPVDescription, newPVUnits);
    plot_->addSeries(newPVName, newPVUnits, itemContainer->data(newPVName));
}



void StripTool::deletePV(const QString &pvName, const QModelIndex &index)
{
    plot_->deleteSeries(pvName);
    itemContainer->deleteItem(pvName);
    pvListModel_->deletePV(index);
}



void StripTool::onShowCheckedPV(const QString &pvName)
{
    plot_->showSeries(pvName);
}



void StripTool::onRemoveUncheckedPV(const QString &pvName)
{
    plot_->hideSeries(pvName);
}
