#include "STListView.h"

STListView::STListView(QWidget *parent) :
    QListView(parent)
{
    setAlternatingRowColors(true);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );
}

STListView::~STListView()
{

}

void STListView::onCustomContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    QAction *action = menu.addAction("Add variable");
    action->setEnabled(false);

    action = menu.exec(mapToGlobal(position));

    if (action) {
        if (action->text() == "Add variable")
            emit addVariable();
    }
}
