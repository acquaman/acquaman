#include "StripChart/StripToolView.h"

StripToolView::StripToolView(QWidget *parent, StripToolModel *model) : QWidget(parent)
{
    model_ = model;
    connect( this, SIGNAL(addNewPV(QString, QString, QString)), model_, SLOT(addPV(QString, QString, QString)) );
    connect( this, SIGNAL(deletePV(QModelIndex)), model_, SLOT(deletePV(QModelIndex)) );
    connect( this, SIGNAL(setPVUpdating(QModelIndex, bool)), model_, SLOT(setPVUpdating(QModelIndex, bool)) );
    connect( this, SIGNAL(setValuesDisplayed(QModelIndex, int)), model_, SLOT(setValuesDisplayed(QModelIndex, int)) );

    createActions();
    buildUI();
}



StripToolView::~StripToolView()
{
}



void StripToolView::createActions()
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

    pausePVAction_ = new QAction("Pause PV", this);
    connect( pausePVAction_, SIGNAL(triggered()), this, SLOT(onPausePVAction()) );

    restartPVAction_ = new QAction("Restart PV", this);
    connect( restartPVAction_, SIGNAL(triggered()), this, SLOT(onRestartPVAction()) );

    deletePVAction_ = new QAction("Delete PV", this);
    connect( deletePVAction_, SIGNAL(triggered()), this, SLOT(onDeletePVAction()) );

    showLessPVAction_ = new QAction("Show Less", this);
    showLessPVAction_->setEnabled(false);
    //connect( showLessPVAction_, SIGNAL(triggered()), this, SLOT(onShowLessPVAction()) );

    showMorePVAction_ = new QAction("Show More", this);
    showMorePVAction_->setEnabled(false);
    //connect( showMorePVAction_, SIGNAL(triggered()), this, SLOT(onShowMorePVAction()) );

    toggleLegendAction_ = new QAction("Plot Legend", this);
    toggleLegendAction_->setEnabled(false);

    editPVAction_ = new QAction("Edit PV", this);
    editPVAction_->setEnabled(false);
}



void StripToolView::buildUI()
{
    plot_ = new StripToolPlot(this);
    connect( model_, SIGNAL(seriesChanged(Qt::CheckState, MPlotItem*)), plot_, SLOT(onSeriesChanged(Qt::CheckState, MPlotItem*)) );

    pvListView_ = new QListView(this);
    pvListView_->setModel(model_);

    pvListView_->setContextMenuPolicy(Qt::ActionsContextMenu);
    pvListView_->addAction(addPVAction_);
    pvListView_->addAction(addSR1CurrentAction_);
    pvListView_->addAction(pausePVAction_);
    pvListView_->addAction(restartPVAction_);
    pvListView_->addAction(deletePVAction_);

    QHBoxLayout *windowLayout = new QHBoxLayout();
    windowLayout->addWidget(plot_);
    windowLayout->addWidget(pvListView_);

    setLayout(windowLayout);
}



void StripToolView::onAddPVAction()
{
    if (addPVDialog_.exec() == QDialog::Accepted)
        emit addNewPV(addPVDialog_.pvName(), addPVDialog_.pvDescription(), addPVDialog_.pvUnits());
}



void StripToolView::onAddSR1CurrentAction()
{
    emit addNewPV("PCT1402-01:mA:fbk", "SR1 Current", "mA");
}



void StripToolView::onPausePVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        emit setPVUpdating(index, false);
    }
}



void StripToolView::onRestartPVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
          emit setPVUpdating(index, true);
    }
}



void StripToolView::onDeletePVAction()
{
    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
    {
        emit deletePV(index);
    }
}



//void StripToolView::onShowLessPVAction()
//{
//    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
//    {
//        int oldAmount = model_->valuesDisplayed(index);
//        int newAmount;

//        if (oldAmount > 5)
//            newAmount = oldAmount - 5;
//        else
//            newAmount = 5;

//        emit setValuesDisplayed(index, newAmount);
//    }
//}



//void StripToolView::onShowMorePVAction()
//{
//    foreach(const QModelIndex &index, pvListView_->selectionModel()->selectedIndexes())
//    {
//        int oldAmount = model_->valuesDisplayed(index);
//        model_->setValuesDisplayed(index, oldAmount += 5);
//    }
//}



//void StripToolView::onEditPVAction()
//{
//}
