#include "StripToolListView.h"

StripToolListView::StripToolListView(QWidget *parent) :
    QListView(parent)
{
    modelSelection_ = 0;

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


    connect( this, SIGNAL(clicked(QModelIndex)), model_, SLOT(itemSelected(QModelIndex)) );
//    connect( this, SIGNAL(newSelection(QModelIndex, QItemSelectionModel::SelectionFlags)), this->selectionModel(), SLOT(select(QModelIndex,QItemSelectionModel::SelectionFlags)) );
    connect( this, SIGNAL(editPV(QList<QModelIndex>)), model_, SLOT(editPV(QList<QModelIndex>)) );
    connect( this, SIGNAL(deletePV(QModelIndex)), model_, SLOT(deletePV(QModelIndex)) );
    connect( this, SIGNAL(setPVUpdating(QModelIndex, bool)), model_, SLOT(setPVUpdating(QModelIndex,bool)) );
    connect( this, SIGNAL(incrementValuesDisplayed(QModelIndex, int)), model_, SLOT(incrementValuesDisplayed(QModelIndex, int)) );
    connect( this, SIGNAL(save(QModelIndex)), model_, SLOT(savePV(QModelIndex)) );

    connect( this, SIGNAL(colorPV(QModelIndex,QColor)), model_, SLOT(colorPV(QModelIndex, QColor)) );
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

    pause_ = new QAction("Pause", this);
    connect( pause_, SIGNAL(triggered()), this, SLOT(pauseSelection()) );

    resume_ = new QAction("Restart", this);
    connect( resume_, SIGNAL(triggered()), this, SLOT(resumeSelection()) );

    setColor_ = new QAction("Line color", this);
    connect( setColor_, SIGNAL(triggered()), this, SLOT(toSetPVColor()) );
}



void StripToolListView::updateContextMenu(const QPoint &position)
{
    QMenu menu(this);

    menu.addAction(edit_);
    menu.addAction(delete_);
    menu.addSeparator();
    menu.addAction(pause_);
    menu.addAction(resume_);
    menu.addSeparator();
    menu.addAction(setColor_);

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
    emit editPV(selectionModel()->selectedIndexes());
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



void StripToolListView::toSetSelection(const QModelIndex &index)
{
    emit newSelection(index, QItemSelectionModel::ClearAndSelect);
}



QColor StripToolListView::colorPicker()
{
    QColorDialog colors;
    QColor color = colors.getColor();
    return color;
}



void StripToolListView::toSetPVColor()
{
    QColor newColor = colorPicker();

    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
    {
        emit colorPV(index, newColor);
    }
}
