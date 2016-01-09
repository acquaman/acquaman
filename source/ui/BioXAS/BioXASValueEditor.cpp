#include "BioXASValueEditor.h"

BioXASValueEditor::BioXASValueEditor(QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize class variables.

	title_ = QString();
	value_ = AMNumber(AMNumber::InvalidError);
	valueText_ = "[Invalid]";
	minimumValue_ = DBL_MIN;
	maximumValue_ = DBL_MAX;
	format_ = 'g';
	precision_ = 3;
	values_ = QStringList();
	units_ = QString();
	readOnly_ = false;

	// Create UI elements.

	valueLabel_ = new QLabel();
	valueLabel_->setAlignment(Qt::AlignCenter);
	valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(255, 255, 255);"); // maybe move this somewhere else?

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(valueLabel_);

	setLayout(layout);

	// Make connections.

	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)) );

	// Current settings.

	setContextMenuPolicy(Qt::CustomContextMenu);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	refresh();
}

BioXASValueEditor::~BioXASValueEditor()
{

}

void BioXASValueEditor::refresh()
{
	updateTitle();
	updateValueLabel();
}

void BioXASValueEditor::updateTitle()
{
	QGroupBox::setTitle(title_);
}

void BioXASValueEditor::updateValueLabel()
{
	valueLabel_->setText( generateValueText() );
}

void BioXASValueEditor::setTitle(const QString &newText)
{
	if (title_ != newText) {
		title_ = newText;
		updateTitle();

		emit titleChanged(title_);
	}
}

void BioXASValueEditor::setValue(double newValue)
{
	if (value_ != newValue) {
		value_ = newValue;
		updateValueLabel();

		emit valueChanged(value_);
	}
}

void BioXASValueEditor::setFormat(const QChar &newFormat)
{
	if (format_ != newFormat && validFormat(newFormat)) {
		format_ = newFormat;
		updateValueLabel();

		emit formatChanged(format_);
	}
}

void BioXASValueEditor::setPrecision(int newPrecision)
{
	if (precision_ != newPrecision && validPrecision(newPrecision)) {
		precision_ = newPrecision;
		updateValueLabel();

		emit precisionChanged(precision_);
	}
}

void BioXASValueEditor::setMinimumValue(double minimumValue)
{
	if (minimumValue_ != minimumValue) {
		minimumValue_ = minimumValue;

		emit minimumValueChanged(minimumValue_);
	}
}

void BioXASValueEditor::setMaximumValue(double maximumValue)
{
	if (maximumValue_ != maximumValue) {
		maximumValue_ = maximumValue;

		emit maximumValueChanged(maximumValue_);
	}
}

void BioXASValueEditor::setValues(const QStringList &newValues)
{
	if (values_ != newValues) {
		values_ = newValues;

		emit valuesChanged(values_);
	}
}

void BioXASValueEditor::setUnits(const QString &newUnits)
{
	if (units_ != newUnits) {
		units_ = newUnits;
		updateValueLabel();

		emit unitsChanged(units_);
	}
}

void BioXASValueEditor::setReadOnly(bool readOnly)
{
	if (readOnly_ != readOnly) {
		readOnly_ = readOnly;

		emit readOnlyChanged(readOnly_);
	}
}

AMNumber BioXASValueEditor::getDoubleValue() const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

//	bool inputOK = false;
//	double newValue = QInputDialog::getDouble(this, QString("Editing %1").arg(title_), QString("New value: "), value_, minimumValue_, maximumValue_, precision_, &inputOK);

//	if (inputOK)
//		result = AMNumber(newValue);

	return result;
}

AMNumber BioXASValueEditor::getEnumValue() const
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

//	bool inputOK = false;
//	QString newValueName = QInputDialog::getItem(this, QString("Editing %1").arg(title_), QString("New value: "), values_, value_, true, &inputOK, 0, 0);

//	if (inputOK) {
//		// convert value name to index.
//	}

	return result;
}

void BioXASValueEditor::onContextMenuRequested(const QPoint &clickPosition)
{
	// Create context menu.

	QMenu contextMenu;

	contextMenu.addAction("Edit");

	// Show menu and identify selected action.

	QAction *selectedAction = contextMenu.exec(mapToGlobal(clickPosition));

	if (selectedAction && selectedAction->text() == "Edit") {

		AMNumber newValue = AMNumber(AMNumber::InvalidError);

		if (values_.isEmpty())
			newValue = getDoubleValue();
		else
			newValue = getEnumValue();

		if (newValue.isValid())
			setValue(double(newValue));
	}
}

bool BioXASValueEditor::validFormat(const QChar &format) const
{
	bool result = false;

	if (format == 'E' || format == 'e' || format == 'F' || format == 'f' || format == 'G' || format == 'g')
		result = true;

	return result;
}

bool BioXASValueEditor::validPrecision(int precision) const
{
	bool result = false;

	if (precision >= 0)
		result = true;

	return result;
}

QString BioXASValueEditor::generateValueText() const
{
	QString text = "[Invalid]";

	if (value_.isValid()) {
		text = QString::number(value_, format_.toAscii(), precision_);

		if (!units_.isEmpty())
			text.append(QString(" %1").arg(units_));
	}

	return text;
}

void BioXASValueEditor::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event)
}

