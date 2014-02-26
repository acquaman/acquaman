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
    if (newSelection != selectedItem_)
    {
        if (newSelection == 0)
        {
            if (selectedItem_)
                selectedItem_->setSelected(false);

            selectedItem_ = 0;
            emit deselected();
            emit itemSelected(selectedItem_);

        } else {

            if (selectedItem_)
                selectedItem_->setSelected(false);

            newSelection->setSelected(true);
            emit itemSelected(selectedItem_ = newSelection);
        }
    }
}
