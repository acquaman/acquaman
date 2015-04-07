#include "CLSStandardsWheelConfigurationView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

// CLSStandardsWheelConfigurationViewElement
////////////////////////////////////////////////////////

CLSStandardsWheelConfigurationViewElement::CLSStandardsWheelConfigurationViewElement(CLSStandardsWheelElement *element, QWidget *parent)
	: QWidget(parent)
{
	wheelElement_ = element;

	nameEdit_ = new QLineEdit(wheelElement_->name());
	nameEdit_->setAlignment(Qt::AlignCenter);

	connect(nameEdit_, SIGNAL(editingFinished()), this, SLOT(onNameChanged()));
	connect(wheelElement_, SIGNAL(nameChanged(QString)), nameEdit_, SLOT(setText(QString)));

	QHBoxLayout	*elementLayout = new QHBoxLayout;
	elementLayout->addWidget(nameEdit_, 0, Qt::AlignCenter);

	setLayout(elementLayout);
}

CLSStandardsWheelConfigurationViewElement::~CLSStandardsWheelConfigurationViewElement()
{
}

void CLSStandardsWheelConfigurationViewElement::onNameChanged()
{
	wheelElement_->setName(nameEdit_->text());
}

// CLSStandardsWheelConfigurationView
////////////////////////////////////////////////////////

CLSStandardsWheelConfigurationView::CLSStandardsWheelConfigurationView(CLSStandardsWheel *standardsWheel, QWidget *parent)
	: QWidget(parent)
{
	standardsWheel_ = standardsWheel;

	QVBoxLayout *wheelLayout = new QVBoxLayout;

	for (int i = 0, size = standardsWheel_->wheelElements().size(); i < size; i++){

		QHBoxLayout *elementLayout = new QHBoxLayout;
		elementLayout->addWidget(new QLabel(QString("%1 :").arg(i+1)));
		elementLayout->addWidget(new CLSStandardsWheelConfigurationViewElement(standardsWheel_->wheelElementAt(i)));
		wheelLayout->addLayout(elementLayout);
	}

	setLayout(wheelLayout);
}

CLSStandardsWheelConfigurationView::~CLSStandardsWheelConfigurationView()
{

}
