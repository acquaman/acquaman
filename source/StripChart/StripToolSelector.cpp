/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
