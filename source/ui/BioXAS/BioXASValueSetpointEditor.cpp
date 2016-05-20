#include "BioXASValueSetpointEditor.h"
#include "float.h"

BioXASValueSetpointEditor::BioXASValueSetpointEditor(InputType type, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	status_ = StatusNone;
	type_ = TypeDouble;

	minimumSet_ = false;
	minimum_ = 0;

	maximumSet_ = false;
	maximum_ = 0;

	// Create UI elements.

	spinBox_ = new QDoubleSpinBox();
	spinBox_->setMinimum(-DBL_MAX);
	spinBox_->setMaximum(DBL_MAX);
	connect( spinBox_, SIGNAL(valueChanged(double)), this, SLOT(onSpinBoxValueChanged()) );

	comboBox_ = new QComboBox();
	connect( comboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxValueChanged()) );

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(spinBox_);
	layout->addWidget(comboBox_);

	setLayout(layout);

	// Current settings.

	setInputType(type);
	setInputStatus(StatusBad);
}

BioXASValueSetpointEditor::~BioXASValueSetpointEditor()
{

}

double BioXASValueSetpointEditor::value() const
{
	double result = 0;

	if (type_ == TypeDouble)
		result = spinBox_->value();
	else if (type_ == TypeEnum)
		result = comboBox_->currentIndex();

	return result;
}

void BioXASValueSetpointEditor::setInputType(InputType newType)
{
	if (type_ != newType) {
		type_ = newType;

		emit inputTypeChanged(type_);
	}

	updateBoxes();
}

void BioXASValueSetpointEditor::setValue(double newValue)
{
	if (type_ == TypeDouble)
		spinBox_->setValue(newValue);
	else if (type_ == TypeEnum)
		comboBox_->setCurrentIndex(newValue);
}

void BioXASValueSetpointEditor::setValues(const QStringList &newValues)
{
	comboBox_->clear();
	comboBox_->addItems(newValues);
}

void BioXASValueSetpointEditor::setMinimum(double newMin)
{
	if (minimum_ != newMin || !minimumSet_) {
		minimumSet_ = true;
		minimum_ = newMin;

		emit minimumChanged(minimum_);
	}

	updateInputStatus();
}

void BioXASValueSetpointEditor::setMaximum(double newMax)
{
	if (maximum_ != newMax || !maximumSet_) {
		maximumSet_ = true;
		maximum_ = newMax;

		emit maximumChanged(maximum_);
	}

	updateInputStatus();
}
#include <QDebug>
void BioXASValueSetpointEditor::setInputStatus(InputStatus newStatus)
{
	qDebug() << "Setting status to:" << (newStatus == StatusBad ? "Bad" : "None");

	if (status_ != newStatus) {
		status_ = newStatus;

		emit inputStatusChanged(status_);
	}

	style()->unpolish(this);
	style()->polish(this);

	update();
}

void BioXASValueSetpointEditor::updateInputStatus()
{
	qDebug() << "Updating input status.";
//	InputStatus newStatus = StatusNone;

//	if (type_ == TypeDouble && minimumSet_ && spinBox_->value() < minimum_)
//		newStatus = StatusBad;
//	else if (type_ == TypeDouble && maximumSet_ && spinBox_->value() > maximum_)
//		newStatus = StatusBad;
//	else if (type_ == TypeEnum && minimumSet_ && comboBox_->currentIndex() < minimum_)
//		newStatus = StatusBad;
//	else if (type_ == TypeEnum && maximumSet_ && comboBox_->currentIndex() > maximum_)
//		newStatus = StatusBad;

//	setInputStatus(newStatus);

	setInputStatus(StatusBad);
}

void BioXASValueSetpointEditor::updateBoxes()
{
	// Update the spinbox.

	if (type_ == TypeDouble)
		spinBox_->show();
	else
		spinBox_->hide();

	// Update the combobox.

	if (type_ == TypeEnum)
		comboBox_->show();
	else
		comboBox_->hide();
}

void BioXASValueSetpointEditor::onSpinBoxValueChanged()
{
	if (type_ == TypeDouble)
		emit valueChanged(spinBox_->value());
}

void BioXASValueSetpointEditor::onComboBoxValueChanged()
{
	if (type_ == TypeEnum)
		emit valueChanged(comboBox_->currentIndex());
}
