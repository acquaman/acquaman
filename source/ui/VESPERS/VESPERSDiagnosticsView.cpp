#include "VESPERSDiagnosticsView.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QStringList>

VESPERSDiagnosticsView::VESPERSDiagnosticsView(AMControlSet *set, int offset, bool useValueAndUnits, QWidget *parent)
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

			element = new VESPERSDiagnosticsViewElement(control, temp.join(" "), useValueAndUnits);
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

			element = new VESPERSDiagnosticsViewElement(control, temp.join(" "), useValueAndUnits);

			if (i < setCount/2)
			layout1->addWidget(element);
			else
			layout2->addWidget(element);
		}

		layout1->addStretch();
		layout2->addStretch();

		QHBoxLayout *sideLayout = new QHBoxLayout;
		sideLayout->addStretch();
		sideLayout->addLayout(layout1);
		sideLayout->addLayout(layout2);
		sideLayout->addStretch();

		setLayout(sideLayout);
	}
}

VESPERSDiagnosticsViewElement::VESPERSDiagnosticsViewElement(AMControl *control, QString name, bool useValueAndUnits, QWidget *parent)
	: QWidget(parent)
{
	control_ = control;
	useValueAndUnits_ = useValueAndUnits;

	QLabel *nameLabel = new QLabel(name);
	nameLabel->setFixedWidth(130);
	nameLabel->setAlignment(Qt::AlignCenter);

	value_ = new QLabel;
	value_->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	value_->setAutoFillBackground(true);
	value_->setAlignment(Qt::AlignCenter);
	value_->setFixedWidth(130);
	connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));

	QFormLayout *layout = new QFormLayout;
	layout->setAlignment(Qt::AlignRight);
	layout->addRow(nameLabel, value_);

	setLayout(layout);
}

void VESPERSDiagnosticsViewElement::onValueChanged(double val)
{
	if (useValueAndUnits_){

		if (control_->units().contains("deg"))
			value_->setText(QString::number(val, 'g', 3)+" "+control_->units().replace("deg ", QString::fromUtf8("°")));
		else if (control_->units().contains("l"))
			value_->setText(QString::number(val, 'g', 3)+" "+control_->units().replace("l", "L"));
		else
			value_->setText(QString::number(val, 'g', 3)+" "+control_->units());

		value_->setPalette(QPalette(!control_->isMoving() ? Qt::green : Qt::red));
	}

	else{

		value_->setText((int)val == 1 ? "GOOD" : "BAD");
		value_->setPalette(QPalette((int)val == 1 ? Qt::green : Qt::red));
	}
}
