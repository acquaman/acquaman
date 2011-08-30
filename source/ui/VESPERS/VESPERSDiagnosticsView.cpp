#include "VESPERSDiagnosticsView.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QStringList>

VESPERSDiagnosticsView::VESPERSDiagnosticsView(AMControlSet *set, int offset, QWidget *parent)
    : QWidget(parent)
{
    AMControl *control;
    QStringList temp;
    VESPERSDiagnosticsViewElement *element;

    int setCount = set->count();

    if (setCount < 10){

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setSpacing(5);

	for (int i = 0; i < set->count(); i++){

	    control = set->at(i);
	    temp = control->name().split(" ");
	    for (int j = 0; j < offset; j++)
		temp.removeFirst();

	    element = new VESPERSDiagnosticsViewElement(control, temp.join(" "));
	    layout->addWidget(element);
	}

	layout->addStretch();
	setLayout(layout);
    }
    else {

	QVBoxLayout *layout1 = new QVBoxLayout;
	layout1->setSpacing(5);
	QVBoxLayout *layout2 = new QVBoxLayout;
	layout2->setSpacing(5);

	for (int i = 0; i < setCount; i++){

	    control = set->at(i);
	    temp = control->name().split(" ");
	    for (int j = 0; j < offset; j++)
		temp.removeFirst();

	    element = new VESPERSDiagnosticsViewElement(control, temp.join(" "));

	    if (i < setCount/2)
		layout1->addWidget(element);
	    else
		layout2->addWidget(element);
	}

	layout1->addStretch();
	layout2->addStretch();

	QHBoxLayout *sideLayout = new QHBoxLayout;
	sideLayout->addLayout(layout1);
	sideLayout->addLayout(layout2);
	setLayout(sideLayout);
    }
}

VESPERSDiagnosticsViewElement::VESPERSDiagnosticsViewElement(AMControl *control, QString name, QWidget *parent)
    : QWidget(parent)
{
    control_ = control;

    value_ = new QLabel;
    connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));

    QFormLayout *layout = new QFormLayout;
    layout->addRow(name, value_);

    setLayout(layout);
}
