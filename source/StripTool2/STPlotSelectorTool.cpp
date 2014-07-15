#include "STPlotSelectorTool.h"

STPlotSelectorTool::STPlotSelectorTool() : MPlotPlotSelectorTool()
{

}

STPlotSelectorTool::~STPlotSelectorTool()
{

}

void STPlotSelectorTool::setSelection(MPlotItem *newSelection)
{
    if (selectedItem_ == newSelection)
        return;

    if (selectedItem_) {
        selectedItem_->setSelected(false);
        selectedItem_ = 0;
        emit deselected();
    }

    if (newSelection) {
        newSelection->setSelected(true);
        emit itemSelected(selectedItem_ = newSelection);
    }
}
