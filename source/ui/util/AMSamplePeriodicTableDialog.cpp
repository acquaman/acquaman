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


#include "AMSamplePeriodicTableDialog.h"

#include "util/AMElement.h"
#include <QLayout>
#include "AMPeriodicTableView.h"
#include <QToolButton>
#include <QDebug>
AMSamplePeriodicTableDialog::AMSamplePeriodicTableDialog(QList<const AMElement *> elements, QWidget *parent)
    : AMPeriodicTableDialog(parent)
{
    AMPeriodicTableView* ptView = 0;
    for(int i = 0; ptView == 0 && i < layout()->count(); i++)
    {
        ptView = dynamic_cast<AMPeriodicTableView*>(layout()->itemAt(i)->widget());
    }
    if(ptView)
    {
        QToolButton* elementButton;
	foreach(const AMElement* element, elements)
        {
	    elementButton = ptView->button(const_cast<AMElement*>(element));
            elementButton->setCheckable(true);
            elementButton->setChecked(true);
        }
    }

}

const AMElement *AMSamplePeriodicTableDialog::getElement(QList<const AMElement *> elements, QWidget *parent)
{
    AMSamplePeriodicTableDialog dialog(elements, parent);

    return dialog.exec() ? dialog.element() : 0;
}
