#include "StripToolListView.h"

StripToolListView::StripToolListView(QWidget *parent) :
    QListView(parent)
{
    edit_ = 0;
    delete_ = 0;
    color_ = 0;

    buildComponents();
    makeConnections();
    defaultSettings();

    qDebug() << "StripToolListView object created.";
}



StripToolListView::~StripToolListView()
{
}



QModelIndex StripToolListView::selectedIndex() const
{
    if (selectionModel()->selectedIndexes().size() > 0)
        return selectionModel()->selectedIndexes().at(0);
    else
        return QModelIndex();
}



void StripToolListView::setSelectedIndex(const QModelIndex &newSelection)
{
    if (newSelection != selectedIndex()) {

        // an empty QModelIndex indicates something has been deselected elsewhere.
        if (newSelection == QModelIndex()) {
            selectionModel()->clearSelection();
            qDebug() << "StripToolListView :: List item deselected.";

        } else if (newSelection.isValid()) {
            selectionModel()->select(QItemSelection(newSelection, newSelection), QItemSelectionModel::ClearAndSelect);
            qDebug() << "StripToolListView :: Index " << newSelection.row() << " selected.";
        }
    }
}



QColor StripToolListView::colorPicker()
{
    QColorDialog colors;
    QColor color = colors.getColor();
    return color;
}



void StripToolListView::updateContextMenu(const QPoint &position)
{
    QMenu menu(this);

    menu.addAction(edit_);
    menu.addAction(delete_);
    menu.addSeparator();
    menu.addAction(color_);

    menu.exec(mapToGlobal(position));
}



void StripToolListView::editSelection()
{
    foreach (const QModelIndex &index, selectionModel()->selectedIndexes()) {
        emit itemToEdit(index);
    }
}



void StripToolListView::deleteSelection()
{
    foreach(const QModelIndex &index, selectionModel()->selectedIndexes()) {
        emit itemToDelete(index);
    }
}



void StripToolListView::colorSelection()
{
    QColor newColor = colorPicker();

    foreach(const QModelIndex &index, selectionModel()->selectedIndexes())
        emit itemToColor(index, newColor);
}



void StripToolListView::onSelectionChanged(QItemSelection newSelection, QItemSelection oldSelection)
{
    if (newSelection == oldSelection) {
        return;
    }

    QList<QModelIndex> selectedIndices = newSelection.indexes();
    emit listSelectionChanged(selectedIndices.at(0));
}



void StripToolListView::buildComponents()
{
    edit_ = new QAction("Edit", this);
    delete_ = new QAction("Delete", this);
    color_ = new QAction("Line color", this);
}



void StripToolListView::makeConnections()
{
    connect( edit_, SIGNAL(triggered()), this, SLOT(editSelection()) );
    connect( delete_, SIGNAL(triggered()), this, SLOT(deleteSelection()) );
    connect( color_, SIGNAL(triggered()), this, SLOT(colorSelection()) );

    connect( this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(updateContextMenu(const QPoint &)) );
}



void StripToolListView::defaultSettings()
{
    setViewMode(QListView::ListMode);
    setSelectionMode(QListView::SingleSelection);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setMaximumWidth(200);
    setContextMenuPolicy(Qt::CustomContextMenu);
}
