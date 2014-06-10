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
