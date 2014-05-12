#include "StripToolSelector.h"
#include <QDebug>

StripToolSelector::StripToolSelector() :
    MPlotPlotSelectorTool()
{
    makeConnections();
}



StripToolSelector::~StripToolSelector()
{
}



void StripToolSelector::setSelection(MPlotItem *newSelection)
{
    if (newSelection == 0)
        deselectItem();

    else
        selectItem(newSelection);
}



void StripToolSelector::onItemDeselected()
{
    emit itemSelected(0);
}


void StripToolSelector::deselectItem()
{
    if (selectedItem_ != 0) {
        selectedItem_->setSelected(false);
        selectedItem_ = 0;

        qDebug() << "StripToolSelector :: plot item deselected.";
        emit deselected();
    }
}



void StripToolSelector::selectItem(MPlotItem *newSelection)
{
    // assume that the newSelection is nonzero.

    if (newSelection != selectedItem_) {
        newSelection->setSelected(true);
        selectedItem_ = newSelection;

        qDebug() << "StripToolSelector :: plot item selected.";
        emit itemSelected(selectedItem_);
    }
}



void StripToolSelector::makeConnections()
{
    connect( this, SIGNAL(deselected()), this, SLOT(onItemDeselected()) );
}
