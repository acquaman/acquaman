#include "BioXASValueEditor.h"
#include "float.h"

BioXASValueEditor::BioXASValueEditor(QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize class variables.

	value_ = AMNumber(AMNumber::InvalidError);
	minimumValue_ = - DBL_MAX;
	maximumValue_ = DBL_MAX;
	format_ = 'g';
	precision_ = 3;
	readOnly_ = false;
	progressValueMinimum_ = 0;
	progressValueMaximum_ = 100;
	progressValue_ = 0;
	displayProgress_ = true;

	editAction_ = new QAction("Edit", this);

	// Create UI elements.

	valueLabel_ = new BioXASValueProgressLabel();
	valueLabel_->setAlignment(Qt::AlignCenter);
	valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(255, 255, 255);");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(valueLabel_);

	setLayout(layout);

	// Make connections.

	connect( editAction_, SIGNAL(triggered(bool)), this, SLOT(onEditActionTriggered()) );
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
	updateEditAction();
}

void BioXASValueEditor::setTitle(const QString &newText)
{
	if (title_ != newText) {
		title_ = newText;
		updateTitle();

		emit titleChanged(title_);
	}
}

void BioXASValueEditor::setValue(const AMNumber &newValue)
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
		updateValueLabel();

		emit valuesChanged(values_);
	}
}

void BioXASValueEditor::setMoveValues(const QStringList &newValues)
{
	if (moveValues_ != newValues) {
		moveValues_ = newValues;

		emit moveValuesChanged(moveValues_);
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

void BioXASValueEditor::setProgressValueMinimum(double newValue)
{
	if (progressValueMinimum_ != newValue) {
		progressValueMinimum_ = newValue;
		updateValueLabel();

		emit progressValueMinimumChanged(progressValueMinimum_);
	}
}

void BioXASValueEditor::setProgressValueMaximum(double newValue)
{
	if (progressValueMaximum_ != newValue) {
		progressValueMaximum_ = newValue;
		updateValueLabel();

		emit progressValueMaximumChanged(progressValueMaximum_);
	}
}

void BioXASValueEditor::setProgressValue(double newValue)
{
	if (progressValue_ != newValue) {
		progressValue_ = newValue;
		updateValueLabel();

		emit progressValueChanged(progressValue_);
	}
}

void BioXASValueEditor::setDisplayProgress(bool showProgress)
{
	if (displayProgress_ != showProgress) {
		displayProgress_ = showProgress;
		updateValueLabel();

		emit displayProgressChanged(displayProgress_);
	}
}

void BioXASValueEditor::updateTitle()
{
	QGroupBox::setTitle(title_);
}

void BioXASValueEditor::updateValueLabel()
{
	valueLabel_->setText( generateValueText() );
	valueLabel_->setProgressValueMinimum(progressValueMinimum_);
	valueLabel_->setProgressValueMaximum(progressValueMaximum_);
	valueLabel_->setProgressValue(progressValue_);
	valueLabel_->setDisplayProgress(displayProgress_);
}

void BioXASValueEditor::updateEditAction()
{
	bool enabled = false;

	if (!readOnly_)
		enabled = true;

	editAction_->setEnabled(true);
}

AMNumber BioXASValueEditor::getDoubleValue()
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	QString dialogTitle = (title_.isEmpty()) ? QString("Edit value") : QString("Editing %1").arg(title_);
	bool inputOK = false;

	double newValue = QInputDialog::getDouble(this, dialogTitle, QString("New value: "), double(value_), minimumValue_, maximumValue_, precision_, &inputOK);

	if (inputOK)
		result = AMNumber(newValue);

	return result;
}

AMNumber BioXASValueEditor::getEnumValue()
{
	AMNumber result = AMNumber(AMNumber::InvalidError);

	QString dialogTitle = (title_.isEmpty()) ? QString("Edit value") : QString("Editing %1").arg(title_);
	bool inputOK = false;

	QString newValueName = QInputDialog::getItem(this, dialogTitle, QString("New value: "), moveValues_, int(value_), false, &inputOK);

	if (inputOK) {
		int newValueIndex = values_.indexOf(newValueName);

		if (newValueIndex > -1)
			result = AMNumber(newValueIndex);
	}

	return result;
}

void BioXASValueEditor::onContextMenuRequested(const QPoint &clickPosition)
{
	// Update the action.

	updateEditAction();

	// Create context menu.

	QMenu contextMenu;

	contextMenu.addAction(editAction_);

	// Show menu.

	contextMenu.exec(mapToGlobal(clickPosition));
}

void BioXASValueEditor::onEditActionTriggered()
{
	if (!readOnly_) {
		AMNumber newValue = AMNumber(AMNumber::InvalidError);

		if (values_.isEmpty())
			newValue = getDoubleValue();
		else
			newValue = getEnumValue();

		if (newValue.isValid())
			setValue(newValue);

	} else {

		QApplication::beep();
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
	QString text = "[Invalid value]";

	if (value_.isValid()) {

		if (values_.isEmpty()) {
			text = QString("%1%2").arg(QString::number(value_, format_.toAscii(), precision_)).arg(generateUnitsText());

		} else {

			if (int(value_) >= 0 && int(value_) < values_.count())
				text = QString("%1%2").arg(values_.at(int(value_))).arg(generateUnitsText());
			else
				text = QString("[Out of range]");
		}
	}

	return text;
}

QString BioXASValueEditor::generateUnitsText() const
{
	QString text = "";

	if (!units_.isEmpty())
		text = QString(" %1").arg(units_);

	return text;
}

void BioXASValueEditor::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event)

	editAction_->trigger();
}
