#include "AMCurrentAmplifierSingleView.h"

AMCurrentAmplifierSingleView::AMCurrentAmplifierSingleView(AMCurrentAmplifier *amplifier, QWidget *parent) :
	AMCurrentAmplifierView(parent)
{
	amplifier_ = amplifier;
	connect( amplifier_, SIGNAL(amplifierModeChanged()), this, SLOT(refreshView()) );
	connect( amplifier_, SIGNAL(valueChanged()), this, SLOT(onAmplifierValueChanged()) );
	connect( amplifier_, SIGNAL(minimumValue(bool)), minus_, SLOT(setDisabled(bool)) );
	connect( amplifier_, SIGNAL(maximumValue(bool)), plus_, SLOT(setDisabled(bool)) );
	connect( amplifier_, SIGNAL(isConnected(bool)), this, SLOT(refreshView()));

	gainViewAction_ = new QAction("Gain view", this);
	sensitivityViewAction_ = new QAction("Sensitivity view", this);
	connect(gainViewAction_, SIGNAL(triggered()), this, SLOT(onGainViewActionTriggered()));
	connect(sensitivityViewAction_, SIGNAL(triggered()), this, SLOT(onSensitivityViewActionTriggered()));

	refreshView();
}

AMCurrentAmplifierSingleView::~AMCurrentAmplifierSingleView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierSingleView::amplifier() const
{
	return amplifier_;
}

bool AMCurrentAmplifierSingleView::isValid() const
{
	if (amplifier_ && amplifier_->isConnected())
		return true;
	else
		return false;
}

void AMCurrentAmplifierSingleView::onAmplifierValueChanged()
{
	if (isValid()) {
		amplifier_->blockSignals(true);

		int newIndex = value_->findText( toDisplay(amplifier_->value(), amplifier_->units()) );

		if (newIndex != -1)
			value_->setCurrentIndex(newIndex);

		amplifier_->blockSignals(false);
	}
}

void AMCurrentAmplifierSingleView::onValueComboBoxChangedImplementation(const QString &newText)
{
	amplifier_->setValue(newText);
}

void AMCurrentAmplifierSingleView::onMinusClickedImplementation()
{
	amplifier_->decreaseValue();
}

void AMCurrentAmplifierSingleView::onPlusClickedImplementation()
{
	amplifier_->increaseValue();
}

void AMCurrentAmplifierSingleView::refreshViewImplementation()
{
	refreshValues();
	refreshButtons();
}

void AMCurrentAmplifierSingleView::onCustomContextMenuActionImplementation(QMenu &contextMenu)
{
////	if (isValid()) {
//		QMenu contextMenu(this);
//		contextMenu.addAction(basicViewAction_);
//		contextMenu.addAction(advancedViewAction_);

		if (amplifier_ && amplifier_->supportsGainMode() && amplifier_->supportsSensitivityMode() && viewMode_ == AMCurrentAmplifierView::Advanced) {
			menu.addSeparator();
			menu.addAction(gainViewAction_);
			menu.addAction(sensitivityViewAction_);
		}
//		contextMenu.exec(mapToGlobal(position));

////		QMenu menu(this);

////		QAction *basic = menu.addAction("Basic view");
////		basic->setDisabled(viewMode_ == AMCurrentAmplifierView::Basic);

////		QAction *advanced = menu.addAction("Advanced view");
////		advanced->setDisabled(viewMode_ == AMCurrentAmplifierView::Advanced);

////		if (amplifier_ && amplifier_->supportsGainMode() && amplifier_->supportsSensitivityMode() && viewMode_ == AMCurrentAmplifierView::Advanced) {
////			menu.addSeparator();

////			QAction *gain = menu.addAction("Gain view");
////			gain->setDisabled(amplifier_->inGainMode());

////			QAction *sensitivity = menu.addAction("Sensitivity view");
////			sensitivity->setDisabled(amplifier_->inSensitivityMode());
////		}

////		QAction *selected = menu.exec(mapToGlobal(position));

////		if (selected) {
////			if (selected->text() == "Basic view")
////				setViewMode(Basic);

////			else if (selected->text() == "Advanced view")
////				setViewMode(Advanced);

////			else if (selected->text() == "Gain view")
////				amplifier_->setAmplifierMode(AMCurrentAmplifier::Gain);

////			else if (selected->text() == "Sensitivity view")
////				amplifier_->setAmplifierMode(AMCurrentAmplifier::Sensitivity);
////		}
////	}
}

void AMCurrentAmplifierSingleView::onGainViewActionTriggered()
{
	amplifier_->setAmplifierMode(AMCurrentAmplifier::Gain);
}

void AMCurrentAmplifierSingleView::onSensitivityViewActionTriggered()
{
	amplifier_->setAmplifierMode(AMCurrentAmplifier::Sensitivity);
}

void AMCurrentAmplifierSingleView::refreshValues()
{
	value_->clear();

	double minValue = 0;
	double maxValue = 0;

	// (re)populate value_ with appropriate options provided by the amplifier.
	QStringList unitsList = amplifier_->unitsList();

	QList<double> valuesList = amplifier_->values();

	foreach (QString units, unitsList) {
		foreach (double value, valuesList) {

			minValue = amplifier_->minimumValueForUnits(units);
			maxValue = amplifier_->maximumValueForUnits(units);

			if (value >= minValue && value <= maxValue) {
				QString item = toDisplay(value, units);
				value_->addItem(item);
			}
		}
	}

	// values displayed should represent the amplifier's current state.
	onAmplifierValueChanged();
}

void AMCurrentAmplifierSingleView::refreshButtons()
{
	minus_->setDisabled(amplifier_->atMinimumValue());
	plus_->setDisabled(amplifier_->atMaximumValue());
}
