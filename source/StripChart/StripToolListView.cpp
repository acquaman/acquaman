#include "StripToolListView.h"

StripToolListView::StripToolListView(QWidget *parent) :
    QListView(parent)
{
    model_ = 0;

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMaximumWidth(200);

    createActions();
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMovement(QListView::Static); // right now, user cannot reorder list view items -- they can't be moved!

    connect( this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(updateContextMenu(const QPoint &)) );
    connect( this, SIGNAL(setCurrentSelection(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)) );
}



StripToolListView::~StripToolListView()
{
}



void StripToolListView::setPVModel(StripToolModel *model)
{
    model_ = model;

    setModel(model_);

    connect( model_, SIGNAL(modelSelectionChange()), this, SLOT(onModelSelectionChange()) );

    connect( this->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), model_, SLOT(listItemSelected(QModelIndex, QModelIndex)) );

    connect( this, SIGNAL(editPV(QModelIndex)), model_, SLOT(editPV(QModelIndex)) );
    connect( this, SIGNAL(deletePV(QModelIndex)), model_, SLOT(toDeletePV(QModelIndex)) );
//    connect( this, SIGNAL(setPVUpdating(QModelIndex, bool)), model_, SLOT(setPVUpdating(QModelIndex,bool)) );
    connect( this, SIGNAL(colorPV(QModelIndex,QColor)), model_, SLOT(colorPV(QModelIndex, QColor)) );
}



void StripToolListView::createActions()
{
    edit_ = new QAction("Edit", this);
    connect( edit_, SIGNAL(triggered()), this, SLOT(editSelection()) );

    delete_ = new QAction("Delete", this);
    connect( delete_, SIGNAL(triggered()), this, SLOT(deleteSelection()) );

    pause_ = new QAction("Pause", this);
    pause_->setEnabled(false);
//    connect( pause_, SIGNAL(triggered()), this, SLOT(pauseSelection()) );

    resume_ = new QAction("Resume", this);
    resume_->setEnabled(false);
//    connect( resume_, SIGNAL(triggered()), this, SLOT(resumeSelection()) );

    setColor_ = new QAction("Line color", this);
    connect( setColor_, SIGNAL(triggered()), this, SLOT(toSetPVColor()) );
}



void StripToolListView::updateContextMenu(const QPoint &position)
{
    QMenu menu(this);

    menu.addAction(edit_);
    menu.addAction(delete_);
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
    foreach (const QModelIndex &index, selectionModel()->selectedIndexes())
        emit editPV(index);
}



void StripToolListView::pauseSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
        emit setPVUpdating(index, false);
}



void StripToolListView::resumeSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
          emit setPVUpdating(index, true);
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
        emit colorPV(index, newColor);
}



void StripToolListView::onModelSelectionChange()
{
    QModelIndex modelSelection = model_->selectedIndex();

    // an invalid index indicates that there is no model selection--something has been deselected.
    if (modelSelection == QModelIndex())
    {
        qDebug() << "List view item deselected.";
        emit setCurrentSelection(modelSelection);

    } else if (modelSelection.isValid()) {
        qDebug() << "Setting list view selection...";
        emit setCurrentSelection(modelSelection);
    }
}
