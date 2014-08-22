#include "STListView.h"

STListView::STListView(QWidget *parent) : QListView(parent)
{
    setViewMode(QListView::ListMode);
    setSelectionMode(QListView::SingleSelection);
    setAlternatingRowColors(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );
}

STListView::~STListView()
{

}

QModelIndexList STListView::selection() const
{
    return selectionModel()->selectedIndexes();
}

void STListView::setSelectedIndices(const QModelIndexList &newSelection)
{
    if (selectionModel()->selectedIndexes() != newSelection) {

        // an empty newSelection means everything must be deselected.
        if (newSelection.size() == 0) {
            selectionModel()->clearSelection();

        } else {
            selectionModel()->select(QItemSelection(newSelection.first(), newSelection.last()), QItemSelectionModel::ClearAndSelect);
        }
    }
}

void STListView::onCustomContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    QAction *action = menu.addAction("Add variable");

    action = menu.exec(mapToGlobal(position));

    if (action) {
        if (action->text() == "Add variable")
            emit addVariable();
    }
}
