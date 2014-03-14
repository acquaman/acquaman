#include "StripTool.h"

StripTool::StripTool(QWidget *parent) : QWidget(parent)
{
    appDirectory_ = 0;

    model_ = 0;
    mainView_ = 0;
    importer_ = 0;
    exporter_ = 0;
    editor_ = 0;

    setAppDirectory(QDir(QDir::homePath()).filePath("StripTool"));

    buildComponents();
    makeConnections();
    defaultSettings();

    QVBoxLayout *windowLayout = new QVBoxLayout();
    windowLayout->addWidget(mainView_);

    setLayout(windowLayout);
    resize(800, 500);

    qDebug() << "StripTool object created.";
}



StripTool::~StripTool()
{
}



StripToolModel* StripTool::model() const {
    return model_;
}



StripToolView* StripTool::mainView() const
{
    return mainView_;
}



StripToolVariableInfoEditor* StripTool::editor() const
{
    return editor_;
}



StripToolVariableInfoImporter* StripTool::importer() const
{
    return importer_;
}



StripToolVariableInfoExporter* StripTool::exporter() const
{
    return exporter_;
}



QDir StripTool::appDirectory() const {
    return appDirectory_;
}



QDir StripTool::infoImportDirectory() const
{
    return QDir(appDirectory().filePath("info"));
}



QDir StripTool::infoExportDirectory() const
{
    return QDir(appDirectory().filePath("info"));
}



QDir StripTool::dataExportDirectory() const
{
    return QDir(appDirectory().filePath("data"));
}



void StripTool::setAppDirectory(const QDir &newDir)
{
    appDirectory_ = newDir;

    if (!appDirectory_.exists())
        QDir(QDir::homePath()).mkdir("StripTool");
}



void StripTool::onModelItemAdded(const QModelIndex &parent, int rowStart, int rowFinish)
{
    Q_UNUSED(parent)

    // the list view handles adding new items automatically.

    // but we need to deal with the plot view ourselves!
    // add each new item to the plot, if it is checked.

    bool addComplete = false;
    int row = rowStart;

    while (!addComplete && row <= rowFinish) {
        StripToolVariableInfo *info = model()->findVariable(row)->info();

        if (info->checkState() == Qt::Checked) {
            mainView()->plotView()->addPlotItem(model()->plotItem(row));
        }

        row++;
    }
}



void StripTool::onModelItemRemoved(const QModelIndex &parent, int rowStart, int rowFinish) {
    Q_UNUSED(parent)

    // the lsit view handles removing old items automatically.

    // but we need to deal with the plot view ourselves!
    // remove each item from the plot.

    for (int row = rowStart; row < rowFinish; row++) {
        mainView()->plotView()->removePlotItem(model()->plotItem(row));
    }
}



void StripTool::onModelSelectionChanged() {
    // update the plot and list view's selection.
    StripToolVariable* selection = model()->selectedVariable();

    if (selection) {
        qDebug() << "StripTool :: setting new selection for plot and list.";

        // a new variable has been selected.
        MPlotItem *selectedItem = selection->plotItem();
        StripToolVariableInfo* selectedInfo = selection->info();
        QModelIndex selectedIndex = selection->index();

        QString leftAxisName = selectedInfo->description() + " [" + selectedInfo->units() + "]";
        mainView()->plotView()->setLeftAxisName(leftAxisName);

        mainView()->plotView()->setSelectedItem(selectedItem);
        mainView()->listView()->setSelectedIndex(selectedIndex);

    } else {
        qDebug() << "StripTool :: deselecting items from plot and list.";

        // the previous selected variable was deselected.
        QString leftAxisName = "[]";
        mainView()->plotView()->setLeftAxisName(leftAxisName);

        mainView()->plotView()->setSelectedItem(0);
        mainView()->listView()->setSelectedIndex(QModelIndex());
    }
}



void StripTool::onPlotSelectionChanged(MPlotItem* plotSelection) {
    // update the model's selection.
    StripToolVariable *newSelection = model()->findVariable(plotSelection);
    model()->setSelectedVariable(newSelection);

}



void StripTool::onListSelectionChanged (const QModelIndex &listSelection) {
    // update the model's selection.
    StripToolVariable *newSelection = model()->findVariable(listSelection);
    model()->setSelectedVariable(newSelection);
}



void StripTool::onModelSelectionInfoChanged() {
    StripToolVariableInfo *changedInfo = model()->selectedVariable()->info();

//    qDebug() << "StripTool::onModelSelectionInfoChanged() : variable info to export to file : " << changedInfo->name();
//    toExportVariables(changedInfo);

    // we want the plot to reflect any changes in the info's description and/or units.
    QString leftAxisName = changedInfo->description() + " [" + changedInfo->units() + "]";
    mainView()->plotView()->setLeftAxisName(leftAxisName);
}



void StripTool::toImportVariables()
{
    importer_->setImportDirectory(infoImportDirectory());
    importer_->importInfos();
}



void StripTool::toExportVariables(StripToolVariableInfo *toExport)
{
    exporter_->setExportDirectory(infoExportDirectory());
    exporter_->exportInfo(toExport);
}



void StripTool::toEditListItem(const QModelIndex &listItem) {
    editor_->editInfo( model()->findVariable(listItem)->info() );
    qDebug() << "StripTool :: editing complete.";
}



void StripTool::onModelVariableCheckStateChanged(const QModelIndex &index)
{
    // remove the series from the plot, if the variable is unchecked. Add otherwise.
    StripToolVariable *changed = model()->findVariable(index);

    if (changed->info()->checkState() == Qt::Checked) {
        qDebug() << "StripTool :: adding checked variable to plot.";
        mainView()->plotView()->addPlotItem(changed->plotItem());

        // if the new plot item is the selected model variable, select it on the plot!
        if (model()->selectedVariable() == changed) {
            mainView()->plotView()->setSelectedItem(changed->plotItem());
        }

    } else {
        qDebug() << "StripTool :: removing unchecked variable from plot.";
        mainView()->plotView()->removePlotItem(changed->plotItem());
    }
}



void StripTool::addVariableToModel(const QString &name)
{
    StripToolVariableInfo *newInfo = new StripToolVariableInfo();
    newInfo->setName(name);

    model()->addVariable(newInfo);
}



void StripTool::buildComponents() {
    model_ = new StripToolModel(this);
    mainView_ = new StripToolView(this);
    importer_ = new StripToolVariableInfoImporter(this);
    exporter_ = new StripToolVariableInfoExporter(this);
    editor_ = new StripToolVariableInfoEditor(this);
}



void StripTool::makeConnections() {
    connect( model(), SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChanged()) );
    connect( model(), SIGNAL(selectedVariableInfoChanged()), this, SLOT(onModelSelectionInfoChanged()) );
    connect( model(), SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onModelItemAdded(QModelIndex,int,int)) );
    connect( model(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onModelItemRemoved(QModelIndex, int, int)) );
    connect( model(), SIGNAL(variableCheckStateChanged(QModelIndex)), this, SLOT(onModelVariableCheckStateChanged(QModelIndex)) );
    connect( model(), SIGNAL(selectedVariableDisplayRangeChanged(const MPlotAxisRange*)), mainView()->plotView(), SLOT(setLeftAxisRange(const MPlotAxisRange*)) );

    connect( model(), SIGNAL(selectedVariableDisplayRangeChanged(const MPlotAxisRange*)), mainView()->plotView(), SLOT(setLeftAxisRange(const MPlotAxisRange*)) );

    connect( mainView()->nameEntry(), SIGNAL(entryComplete(QString)), this, SLOT(addVariableToModel(QString)) );
    connect( mainView()->waterfallEntry(), SIGNAL(waterfallOn(bool)), model(), SLOT(enableWaterfall(bool)) );
    connect( mainView()->timeEntry(), SIGNAL(timeAmountChanged(int)), model(), SLOT(changeDisplayedTimeAmount(int)) );
    connect( mainView()->timeEntry(), SIGNAL(timeUnitsChanged(QString)), model(), SLOT(changeDisplayedTimeUnits(QString)) );
    connect( mainView()->plotView(), SIGNAL(selectionChanged(MPlotItem*)), this, SLOT(onPlotSelectionChanged(MPlotItem*)) );
    connect( mainView()->listView(), SIGNAL(itemToEdit(QModelIndex)), this, SLOT(toEditListItem(QModelIndex)) );
    connect( mainView()->listView(), SIGNAL(clicked(QModelIndex)), this, SLOT(onListSelectionChanged(QModelIndex)) );

    connect( importer(), SIGNAL(importedInfo(StripToolVariableInfo*)), model(), SLOT(addVariable(StripToolVariableInfo*)) );
}



void StripTool::defaultSettings() {    
    mainView()->plotView()->setBottomAxisName("Time []");
    mainView()->plotView()->setLeftAxisName("");

    mainView()->listView()->setModel(model());

    importer()->setImportDirectory(infoImportDirectory());
    exporter()->setExportDirectory(infoExportDirectory());

    importer()->importInfos();
}

