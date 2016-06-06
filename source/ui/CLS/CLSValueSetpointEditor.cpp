#include "CLSValueSetpointEditor.h"

#include <QStyleOption>
#include <QPainter>
#include "float.h"

CLSValueSetpointEditor::CLSValueSetpointEditor(InputType type, QWidget *parent) :
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

	connect( spinBox_, SIGNAL(valueChanged(double)), this, SLOT(onBoxValueChanged()) );

	comboBox_ = new QComboBox();

	connect( comboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onBoxValueChanged()) );

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(spinBox_);
	layout->addWidget(comboBox_);

	setLayout(layout);

	// Current settings.

	setInputType(type);
}

CLSValueSetpointEditor::~CLSValueSetpointEditor()
{

}

double CLSValueSetpointEditor::value() const
{
	double result = 0;

	if (type_ == TypeDouble)
		result = spinBox_->value();
	else if (type_ == TypeEnum)
		result = comboBox_->currentIndex();

	return result;
}

bool CLSValueSetpointEditor::valueLessThanMinimum() const
{
	return (minimumSet_ && getValue() < minimum_);
}

bool CLSValueSetpointEditor::valueGreaterThanMaximum() const
{
	return (maximumSet_ && getValue() > maximum_);
}

void CLSValueSetpointEditor::setInputType(InputType newType)
{
	if (type_ != newType) {
		type_ = newType;

		emit inputTypeChanged(type_);
	}

	updateBoxes();
}

void CLSValueSetpointEditor::setValue(double newValue)
{
	if (type_ == TypeDouble)
		spinBox_->setValue(newValue);
	else if (type_ == TypeEnum)
		comboBox_->setCurrentIndex(newValue);
}

void CLSValueSetpointEditor::setValues(const QStringList &newValues)
{
	comboBox_->clear();
	comboBox_->addItems(newValues);
}

void CLSValueSetpointEditor::setMinimum(double newMin)
{
	if (minimum_ != newMin || !minimumSet_) {
		minimumSet_ = true;
		minimum_ = newMin;

		emit minimumChanged(minimum_);
	}

	updateInputStatus();
}

void CLSValueSetpointEditor::setMaximum(double newMax)
{
	if (maximum_ != newMax || !maximumSet_) {
		maximumSet_ = true;
		maximum_ = newMax;

		emit maximumChanged(maximum_);
	}

	updateInputStatus();
}

void CLSValueSetpointEditor::setInputStatus(InputStatus newStatus)
{
	if (status_ != newStatus) {
		status_ = newStatus;

		emit inputStatusChanged(status_);
	}

	// Trigger style update.

	style()->unpolish(this);
	style()->polish(this);

	update();

	// Update the tooltip.

	updateToolTip();
}

void CLSValueSetpointEditor::updateInputStatus()
{
	setInputStatus( getInputStatus() );
}

void CLSValueSetpointEditor::updateToolTip()
{
	setToolTip( getToolTipText() );
}

void CLSValueSetpointEditor::updateBoxes()
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

void CLSValueSetpointEditor::onBoxValueChanged()
{
	updateInputStatus();
	emit valueChanged( getValue() );
}

void CLSValueSetpointEditor::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

double CLSValueSetpointEditor::getValue() const
{
	double result = -1;

	if (type_ == TypeDouble)
		result = spinBox_->value();
	else if (type_ == TypeEnum)
		result = comboBox_->currentIndex();

	return result;
}

CLSValueSetpointEditor::InputStatus CLSValueSetpointEditor::getInputStatus() const
{
	InputStatus newStatus = StatusNone;

	if (type_ == TypeDouble && valueLessThanMinimum())
		newStatus = StatusBad;
	else if (type_ == TypeDouble && valueGreaterThanMaximum())
		newStatus = StatusBad;
	else if (type_ == TypeEnum && valueLessThanMinimum())
		newStatus = StatusBad;
	else if (type_ == TypeEnum && valueGreaterThanMaximum())
		newStatus = StatusBad;

	return newStatus;
}

QString CLSValueSetpointEditor::getToolTipText() const
{
	QString newText = "";

	if (status_ == StatusBad && valueLessThanMinimum())
		newText = QString("Value is less than %1.").arg(minimum_);
	else if (status_ == StatusBad && valueGreaterThanMaximum())
		newText = QString("Value is greater than %1.").arg(maximum_);

	return newText;
}
