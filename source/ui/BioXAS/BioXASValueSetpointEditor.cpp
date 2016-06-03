#include "BioXASValueSetpointEditor.h"
#include "float.h"
#include <QStyleOption>
#include <QPainter>

BioXASValueSetpointEditor::BioXASValueSetpointEditor(InputType type, bool showFeedback, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	status_ = StatusNone;
	type_ = TypeDouble;
	minimumSet_ = false;
	minimum_ = 0;
	maximumSet_ = false;
	maximum_ = 0;
	displayValueFeedback_ = false;
	valueFeedback_ = 0;
	precision_ = 3;

	// Create UI elements.

	spinBox_ = new QDoubleSpinBox();
	spinBox_->setMinimum(-DBL_MAX);
	spinBox_->setMaximum(DBL_MAX);
	connect( spinBox_, SIGNAL(valueChanged(double)), this, SLOT(onBoxValueChanged()) );

	comboBox_ = new QComboBox();
	connect( comboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onBoxValueChanged()) );

	feedbackLabel_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(spinBox_);
	layout->addWidget(comboBox_);
	layout->addWidget(feedbackLabel_);

	setLayout(layout);

	// Current settings.

	setInputType(type);
	setDisplayValueFeedback(showFeedback);
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

bool BioXASValueSetpointEditor::valueLessThanMinimum() const
{
	return (minimumSet_ && getValue() < minimum_);
}

bool BioXASValueSetpointEditor::valueGreaterThanMaximum() const
{
	return (maximumSet_ && getValue() > maximum_);
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

	updateInputStatus();
}

void BioXASValueSetpointEditor::setDisplayValueFeedback(bool showFeedback)
{
	if (displayValueFeedback_ != showFeedback) {
		displayValueFeedback_ = showFeedback;
		emit displayFeedbackValueChanged(displayValueFeedback_);
	}

	updateValueFeedbackLabel();
}

void BioXASValueSetpointEditor::setValueFeedback(double newValue)
{
	if (valueFeedback_ != newValue) {
		valueFeedback_ = newValue;
		emit valueFeedbackChanged(valueFeedback_);
	}

	updateValueFeedbackLabel();
}

void BioXASValueSetpointEditor::setValues(const QStringList &newValues)
{
	comboBox_->clear();
	comboBox_->addItems(newValues);

	updateInputStatus();
}

void BioXASValueSetpointEditor::setMinimumValue(double newMin)
{
	if (minimum_ != newMin || !minimumSet_) {
		minimumSet_ = true;
		minimum_ = newMin;
		emit minimumChanged(minimum_);
	}

	updateInputStatus();
}

void BioXASValueSetpointEditor::setMaximumValue(double newMax)
{
	if (maximum_ != newMax || !maximumSet_) {
		maximumSet_ = true;
		maximum_ = newMax;
		emit maximumChanged(maximum_);
	}

	updateInputStatus();
}

void BioXASValueSetpointEditor::setUnits(const QString &units)
{
	if (units_ != units) {
		units_ = units;
		emit unitsChanged(units_);
	}

	updateBoxes();
	updateValueFeedbackLabel();
}

void BioXASValueSetpointEditor::setPrecision(int precision)
{
	if (precision_ != precision) {
		precision_ = precision;
		emit precisionChanged(precision_);
	}

	updateBoxes();
	updateValueFeedbackLabel();
}

void BioXASValueSetpointEditor::clear()
{
	spinBox_->clear();
	comboBox_->clear();
}

void BioXASValueSetpointEditor::setInputStatus(InputStatus newStatus)
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

void BioXASValueSetpointEditor::updateInputStatus()
{
	setInputStatus( getInputStatus() );
}

void BioXASValueSetpointEditor::updateToolTip()
{
	setToolTip( getToolTipText() );
}

void BioXASValueSetpointEditor::updateBoxes()
{
	// Update the spinbox.

	spinBox_->setSuffix(units_.isEmpty() ? "" : QString(" %1").arg(units_));
	spinBox_->setDecimals(precision_);
	spinBox_->setVisible(type_ == TypeDouble);

	// Update the combobox.

	comboBox_->setVisible(type_ == TypeEnum);
}

void BioXASValueSetpointEditor::updateValueFeedbackLabel()
{
	QString newText = QString::number(valueFeedback_, 'g', precision_);
	if (!units_.isEmpty())
		newText.append(QString(" %1").arg(units_));

	feedbackLabel_->setText(newText);

	feedbackLabel_->setVisible(displayValueFeedback_);
}

void BioXASValueSetpointEditor::onBoxValueChanged()
{
	updateInputStatus();
	emit valueChanged( getValue() );
}

void BioXASValueSetpointEditor::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

double BioXASValueSetpointEditor::getValue() const
{
	double result = -1;

	if (type_ == TypeDouble)
		result = spinBox_->value();
	else if (type_ == TypeEnum)
		result = comboBox_->currentIndex();

	return result;
}

BioXASValueSetpointEditor::InputStatus BioXASValueSetpointEditor::getInputStatus() const
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

QString BioXASValueSetpointEditor::getToolTipText() const
{
	QString newText = "";

	if (status_ == StatusBad && valueLessThanMinimum())
		newText = QString("Value is less than %1.").arg(minimum_);
	else if (status_ == StatusBad && valueGreaterThanMaximum())
		newText = QString("Value is greater than %1.").arg(maximum_);

	return newText;
}
