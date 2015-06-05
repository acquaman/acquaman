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

	QFormLayout *wheelLayout = new QFormLayout;

	standardsComboBox_ = new QComboBox;
	standardsComboBox_->setEditable(true);
	standardsComboBox_->lineEdit()->setAlignment(Qt::AlignCenter);

	foreach (CLSStandardsWheelElement *element, standardsWheel_->wheelElements())
		standardsComboBox_->addItem(element->name());

	connect(standardsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onStandardsWheelIndexChanged(int)));
	connect(standardsWheel_, SIGNAL(nameChanged(int,QString)), this, SLOT(onStandardsWheelNameChanged(int,QString)));
	connect(standardsWheel_, SIGNAL(positionChanged(int)), this, SLOT(onStandardsWheelPositionChanged(int)));

	wheelLayout->addRow("Position", standardsComboBox_);

	for (int i = 0, size = standardsWheel_->wheelElements().size()/2; i < size; i++){

		QHBoxLayout *elementLayout = new QHBoxLayout;
		elementLayout->addWidget(new QLabel(QString("%1 :").arg(i+1)));
		elementLayout->addWidget(new CLSStandardsWheelConfigurationViewElement(standardsWheel_->wheelElementAt(i)));

		elementLayout->addWidget(new QLabel(QString("%1 :").arg(i+size+1)));
		elementLayout->addWidget(new CLSStandardsWheelConfigurationViewElement(standardsWheel_->wheelElementAt(i+size)));

		wheelLayout->addRow(elementLayout);
	}

	setLayout(wheelLayout);
}

CLSStandardsWheelConfigurationView::~CLSStandardsWheelConfigurationView()
{

}

void CLSStandardsWheelConfigurationView::onStandardsWheelIndexChanged(int index)
{
	standardsWheel_->moveToIndex(index);
}

void CLSStandardsWheelConfigurationView::onStandardsWheelNameChanged(int index, const QString &newName)
{
	standardsComboBox_->setItemData(index, newName);
}

void CLSStandardsWheelConfigurationView::onStandardsWheelPositionChanged(int index)
{
	standardsComboBox_->blockSignals(true);
	standardsComboBox_->setCurrentIndex(index);
	standardsComboBox_->blockSignals(false);
}
