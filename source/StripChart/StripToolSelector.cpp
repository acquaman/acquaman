#include "StripToolSelector.h"
#include <QDebug>

StripToolSelector::StripToolSelector() :
    MPlotPlotSelectorTool()
{
}



StripToolSelector::~StripToolSelector()
{
}



void StripToolSelector::setSelection(MPlotItem *newSelection)
{
    if (newSelection != selectedItem()) {
        if (newSelection == 0) {
            deselectItem();

            qDebug() << "StripToolSelector :: plot item deselected.";
            emit deselected();

        } else {
            selectItem(newSelection);

            qDebug() << "StripToolSelector :: plot item selected.";
            emit itemSelected(selectedItem_);
        }
    }
}



void StripToolSelector::deselectItem()
{
    if (selectedItem_ != 0) {
        selectedItem_->setSelected(false);
        selectedItem_ = 0;
    }
}



void StripToolSelector::selectItem(MPlotItem *newSelection)
{
    if (selectedItem_ != 0) {
        deselectItem();
    }

    newSelection->setSelected(true);
    selectedItem_ = newSelection;
}
