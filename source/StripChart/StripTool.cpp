/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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



StripToolVariable* StripTool::createDerivativeFromVariable(StripToolVariable *variable)
{
//    qDebug() << "StripTool :: creating derivative entry from variable : " << variable->info()->name();

//    AMDataSource *baseSource = variable->info()->dataSource();
//    qDebug() << "StripTool :: base source has rank " << baseSource->rank();

//    AM1DDerivativeAB *derivSource = new AM1DDerivativeAB("d/dt - " + variable->info()->name(), this);
//    derivSource->setInputDataSources(QList<AMDataSource*>() << baseSource);

//    StripToolVariableInfo *derivInfo = new StripToolVariableInfo();
//    derivInfo->setDataSource(derivSource);

//    StripToolVariable *derivVariable = new StripToolVariable(derivInfo);


//    qDebug() << "StripTool :: derivative entry created.";

//    return derivVariable;
    return 0;
}



void StripTool::onModelItemAdded(const QModelIndex &parent, int rowStart, int rowFinish)
{
    Q_UNUSED(parent)

    // the list view handles adding new items automatically.
    // but we need to deal with the plot view ourselves!

    for (int row = rowStart; row < rowFinish; row++) {

        // first, we make sure the new item has valid time units/amount to display.
        StripToolVariableInfo *infoAdded = model()->findVariable(row)->info();
        infoAdded->setTimeAmount(mainView()->timeEntry()->timeAmount());
        infoAdded->setTimeUnits(mainView()->timeEntry()->timeUnits());

        // add each new item to the plot, if it is checked.
        if (infoAdded->checkState() == Qt::Checked) {
            mainView()->plotView()->addPlotItem(model()->findVariable(row)->series());
        }
    }
}



void StripTool::onModelItemRemoved(const QModelIndex &parent, int rowStart, int rowFinish) {
    Q_UNUSED(parent)

    // the lsit view handles removing old items automatically.
    // but we need to deal with the plot view ourselves!
    // remove each item from the plot.

    for (int row = rowStart; row < rowFinish; row++) {
        mainView()->plotView()->removePlotItem(model()->findVariable(row)->series());
    }
}



void StripTool::onModelSelectionChanged() {
    // update the plot and list view's selection.
    StripToolVariable* selection = model()->selectedVariable();

    if (selection) {

        // a new variable has been selected.
        StripToolVariableInfo* selectedInfo = selection->info();

        // check that the new selection has the right time units.
        selectedInfo->setTimeUnits(mainView()->timeEntry()->timeUnits());
        selectedInfo->setTimeAmount(mainView()->timeEntry()->timeAmount());

        // all selections have their characteristics reflected on the plot left axis.
        QString leftAxisName = selectedInfo->description() + " [" + selectedInfo->units() + "]";
        mainView()->plotView()->setLeftAxisName(leftAxisName);

        // if the selection is also contained in the plot, it will become the selected plot item.
        mainView()->plotView()->setSelectedItem(selection->series());

        // all variables added to the application will have entries on the list view, whether or not they are plotted.
        // update the list view selection to reflect the change.
        qDebug() << "StripTool::onModelSelectionChange : preparing to select list item at index " << selection->index().row();
        mainView()->listView()->setSelectedIndex(selection->index());

    } else {

        qDebug() << "StripTool :: deselecting items from plot and list.";

        // the previous selected variable was deselected.
        QString leftAxisName = "";
        mainView()->plotView()->setLeftAxisName(leftAxisName);
        mainView()->plotView()->setSelectedItem(0);
        mainView()->listView()->setSelectedIndex(QModelIndex());
    }
}



void StripTool::onPlotSelectionChanged(MPlotItem* plotSelection)
{
    // update the model's selection.
    StripToolVariable *newSelection = model()->findVariable(plotSelection);
    model()->setSelectedVariable(newSelection);
}



void StripTool::onListSelectionChanged (const QModelIndex &listSelection)
{
    // update the model's selection.
    StripToolVariable *newSelection = model()->findVariable(listSelection);
    model()->setSelectedVariable(newSelection);
}



void StripTool::onModelSelectionInfoChanged()
{
    StripToolVariableInfo *changedInfo = model()->selectedVariable()->info();

//    qDebug() << "StripTool::onModelSelectionInfoChanged() : variable info to export to file : " << changedInfo->name();
    toExportVariables(changedInfo);

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



void StripTool::toEditListItem(const QModelIndex &listItem)
{
    editor_->editInfo( model()->findVariable(listItem)->info() );
    qDebug() << "StripTool :: editing complete.";
}



void StripTool::toRemoveListItem(const QModelIndex &listItem)
{
    model()->removeVariable(listItem);
    qDebug() << "StripTool :: remove complete.";
}



void StripTool::toDeleteListItem(const QModelIndex &listItem)
{
    StripToolVariable *toDelete = model()->findVariable(listItem);
    model()->deleteVariable(toDelete);
    qDebug() << "StripTool :: delete complete.";
}



void StripTool::toAddDerivativeListItem(const QModelIndex &listItem)
{
    StripToolVariable *derivVar = createDerivativeFromVariable(model()->findVariable(listItem));
//    model()->addVariable(derivVar);

//    qDebug() << "StripTool :: derivative entry added to model.";
}



void StripTool::onModelVariableCheckStateChanged(const QModelIndex &index)
{
    // remove the series from the plot, if the variable is unchecked. Add otherwise.
    StripToolVariable *changed = model()->findVariable(index);

    if (changed->info()->checkState() == Qt::Checked) {
        qDebug() << "StripTool :: adding checked variable to plot.";
        mainView()->plotView()->addPlotItem(changed->series());

        // if the new plot item is the selected model variable, select it on the plot!
        if (model()->selectedVariable() == changed) {
            mainView()->plotView()->setSelectedItem(changed->series());
        }

    } else {
        qDebug() << "StripTool :: removing unchecked variable from plot.";
        mainView()->plotView()->removePlotItem(changed->series());
    }
}



void StripTool::addVariableToModel(const QString &name)
{
    StripToolVariableInfo *newInfo = new StripToolVariableInfo();
    newInfo->setName(name);
    newInfo->setCheckState(Qt::Checked);

    model()->addVariable(newInfo);
}



void StripTool::onTimeUnitsChanged(TimeEntryWidget::TimeUnits units)
{
    QString newUnits;
    model()->changeDisplayedTimeUnits(units);

    switch (units) {
    case (TimeEntryWidget::Seconds) :
        newUnits = "sec";
        break;

    case (TimeEntryWidget::Minutes) :
        newUnits = "min";
        break;

    case (TimeEntryWidget::Hours) :
        newUnits = "hr";
        break;

    default:
        newUnits = "???";
        qDebug() << "StripTool::onTimeUnitsChanged(int) : unknown units encountered!!";
        break;
    }

    mainView()->plotView()->setBottomAxisName("Time [" + newUnits + "]");
}



void StripTool::toChangeVariableColor(const QModelIndex &index, const QColor &newColor)
{
    model()->findVariable(index)->info()->setColor(newColor);
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

    connect( mainView()->nameEntry(), SIGNAL(entryComplete(QString)), this, SLOT(addVariableToModel(QString)) );
    connect( mainView()->waterfallEntry(), SIGNAL(waterfallOn(bool)), mainView()->plotView(), SLOT(setWaterfall(bool)) );
    connect( mainView()->timeEntry(), SIGNAL(timeAmountChanged(int)), model(), SLOT(changeDisplayedTimeAmount(int)) );
    connect( mainView()->timeEntry(), SIGNAL(timeUnitsChanged(TimeEntryWidget::TimeUnits)), this, SLOT(onTimeUnitsChanged(TimeEntryWidget::TimeUnits)) );

    connect( mainView()->plotView(), SIGNAL(selectionChanged(MPlotItem*)), this, SLOT(onPlotSelectionChanged(MPlotItem*)) );
    connect( mainView()->listView(), SIGNAL(itemToEdit(QModelIndex)), this, SLOT(toEditListItem(QModelIndex)) );
    connect( mainView()->listView(), SIGNAL(itemToRemove(QModelIndex)), this, SLOT(toRemoveListItem(QModelIndex)) );
    connect( mainView()->listView(), SIGNAL(itemToDelete(QModelIndex)), this, SLOT(toDeleteListItem(QModelIndex)) );
    connect( mainView()->listView(), SIGNAL(itemToColor(QModelIndex,QColor)), this, SLOT(toChangeVariableColor(QModelIndex,QColor)) );
    connect( mainView()->listView(), SIGNAL(itemToFindDerivative(QModelIndex)), this, SLOT(toAddDerivativeListItem(QModelIndex)) );
    connect( mainView()->listView(), SIGNAL(clicked(QModelIndex)), this, SLOT(onListSelectionChanged(QModelIndex)) );

    connect( importer(), SIGNAL(importedInfo(StripToolVariableInfo*)), model(), SLOT(addVariable(StripToolVariableInfo*)) );
}



void StripTool::defaultSettings() {
    mainView()->listView()->setModel(model());

    mainView()->plotView()->setBottomAxisName("Time []");
    mainView()->plotView()->setLeftAxisName("");

    mainView()->timeEntry()->setTimeAmount(10);
    mainView()->timeEntry()->setTimeUnits(TimeEntryWidget::Seconds);

    importer()->setImportDirectory(infoImportDirectory());
    exporter()->setExportDirectory(infoExportDirectory());

    importer()->importInfos();
}

