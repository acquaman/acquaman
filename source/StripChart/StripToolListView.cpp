#include "StripToolListView.h"

StripToolListView::StripToolListView(QWidget *parent) :
    QListView(parent)
{
    model_ = 0;
    modelSelection_ = QModelIndex();


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

    connect( this, SIGNAL(editPV(QList<QModelIndex>)), model_, SLOT(editPV(QList<QModelIndex>)) );
    connect( this, SIGNAL(deletePV(QModelIndex)), model_, SLOT(toDeletePV(QModelIndex)) );
    connect( this, SIGNAL(setPVUpdating(QModelIndex, bool)), model_, SLOT(setPVUpdating(QModelIndex,bool)) );
    connect( this, SIGNAL(incrementValuesDisplayed(QModelIndex, int)), model_, SLOT(incrementValuesDisplayed(QModelIndex, int)) );
    connect( this, SIGNAL(colorPV(QModelIndex,QColor)), model_, SLOT(colorPV(QModelIndex, QColor)) );
}



void StripToolListView::createActions()
{
    edit_ = new QAction("Edit", this);
    connect( edit_, SIGNAL(triggered()), this, SLOT(editSelection()) );

    delete_ = new QAction("Delete", this);
    connect( delete_, SIGNAL(triggered()), this, SLOT(deleteSelection()) );

    pause_ = new QAction("Pause", this);
    connect( pause_, SIGNAL(triggered()), this, SLOT(pauseSelection()) );

    resume_ = new QAction("Resume", this);
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



void StripToolListView::setViewSelection()
{
    QModelIndex modelSelection = model_->selectedIndex();

    if (modelSelection == QModelIndex())
    {
        emit setDeselected(modelSelection_, QItemSelectionModel::Deselect);

    } else {
        QItemSelection *items = new QItemSelection(modelSelection, modelSelection);
        emit setSelected(*items, QItemSelectionModel::ClearAndSelect);
    }

    modelSelection_ = modelSelection;
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
