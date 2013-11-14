#include "StripToolSelector.h"

StripToolSelector::StripToolSelector() :
    MPlotPlotSelectorTool()
{

}



StripToolSelector::~StripToolSelector()
{

}



void StripToolSelector::setSelection(MPlotItem *newSelection)
{
    Q_UNUSED(newSelection);

//    if (newSelection)
//    {
//        selectedItem_->setSelected(false);
//        newSelection->setSelected(true);
//        emit itemSelected(selectedItem_ = newSelection);

//    } else {
//        selectedItem_->setSelected(false);
//        selectedItem_ = 0;
//        emit deselected();
//    }
}
