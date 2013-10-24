#include "StripToolListView.h"

StripToolListView::StripToolListView(QWidget *parent) :
    QListView(parent)
{

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMaximumWidth(200);

    createActions();
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect( this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(updateContextMenu(const QPoint &)) );
}



StripToolListView::~StripToolListView()
{
}



void StripToolListView::setPVModel(StripToolModel *model)
{
    model_ = model;

    setModel(model_);

    connect( this, SIGNAL(editPV(QModelIndex)), model_, SLOT(editPV(QModelIndex)) );
    connect( this, SIGNAL(deletePV(QModelIndex)), model_, SLOT(deletePV(QModelIndex)) );
    connect( this, SIGNAL(setPVUpdating(QModelIndex, bool)), model_, SLOT(setPVUpdating(QModelIndex,bool)) );
    connect( this, SIGNAL(incrementValuesDisplayed(QModelIndex, int)), model_, SLOT(incrementValuesDisplayed(QModelIndex, int)) );
//    connect( this, SIGNAL(setAllValuesDisplayed(QModelIndex)), model_, SLOT(setAllValuesDisplayed(QModelIndex)) );
}



void StripToolListView::createActions()
{
    edit_ = new QAction("Edit", this);
    connect( edit_, SIGNAL(triggered()), this, SLOT(editSelection()) );

    delete_ = new QAction("Delete", this);
    connect( delete_, SIGNAL(triggered()), this, SLOT(deleteSelection()) );

    showLess_ = new QAction("Show Less", this);
    connect( showLess_, SIGNAL(triggered()), this, SLOT(showLessSelection()) );

    showMore_ = new QAction("Show More", this);
    connect( showMore_, SIGNAL(triggered()), this, SLOT(showMoreSelection()) );

//    showAll_ = new QAction("Show All", this);
//    connect( showMore_, SIGNAL(triggered()), this, SLOT(showAllSelection()) );

    pause_ = new QAction("Pause", this);
    connect( pause_, SIGNAL(triggered()), this, SLOT(pauseSelection()) );

    resume_ = new QAction("Restart", this);
    connect( resume_, SIGNAL(triggered()), this, SLOT(resumeSelection()) );
}



void StripToolListView::updateContextMenu(const QPoint &position)
{
    QMenu menu(this);

    menu.addAction(edit_);
    menu.addAction(delete_);
    menu.addSeparator();
    menu.addAction(showLess_);
    menu.addAction(showMore_);
//    menu.addAction(showAll_);
    menu.addSeparator();
    menu.addAction(pause_);
    menu.addAction(resume_);

    menu.exec(mapToGlobal(position));
}



void StripToolListView::deleteSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
    {
        emit deletePV(index);
    }
}



void StripToolListView::editSelection()
{
    // only want to edit pvs one at a time right now.
    emit editPV(selectionModel()->selectedIndexes().at(0));
}



void StripToolListView::showLessSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
    {
        int diff = -5;
        emit incrementValuesDisplayed(index, diff);
    }
}



void StripToolListView::showMoreSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
    {
        int diff = 5;
        emit incrementValuesDisplayed(index, diff);
    }
}



//void StripToolListView::showAllSelection()
//{
////    foreach (const QModelIndex &index, selectionModel()->selectedIndexes())
////    {
////        emit setAllValuesDisplayed(index);
////    }
//}




void StripToolListView::pauseSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
    {
        emit setPVUpdating(index, false);
    }
}



void StripToolListView::resumeSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
    {
          emit setPVUpdating(index, true);
    }
}
