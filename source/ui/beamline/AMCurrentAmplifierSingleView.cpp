#include "AMCurrentAmplifierSingleView.h"

AMCurrentAmplifierSingleView::AMCurrentAmplifierSingleView(AMCurrentAmplifier *amplifier, QWidget *parent) :
    AMCurrentAmplifierView(parent)
{
    amplifier_ = amplifier;
    connect( amplifier_, SIGNAL(amplifierModeChanged()), this, SLOT(refreshView()) );
    connect( amplifier_, SIGNAL(valueChanged()), this, SLOT(onAmplifierValueChanged()) );
    connect( amplifier_, SIGNAL(minimumValue(bool)), minus_, SLOT(setDisabled(bool)) );
    connect( amplifier_, SIGNAL(maximumValue(bool)), plus_, SLOT(setDisabled(bool)) );

    refreshView();
}

AMCurrentAmplifierSingleView::~AMCurrentAmplifierSingleView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierSingleView::amplifier() const
{
    return amplifier_;
}

void AMCurrentAmplifierSingleView::refreshView()
{
    refreshValues();
    refreshButtons();
}

void AMCurrentAmplifierSingleView::onAmplifierValueChanged()
{
    amplifier_->blockSignals(true);

    int newIndex = value_->findText( valueToString(amplifier_->value(), amplifier_->units()) );
    if (newIndex != -1)
        value_->setCurrentIndex(newIndex);

    amplifier_->blockSignals(false);
}

void AMCurrentAmplifierSingleView::onValueComboBoxChanged(const QString &newText)
{
    // the initialized_ boolean prevents the display from setting the amplifier value while view is initializing <- undesirable behavior.
    if (initialized_) {
        amplifier_->setValue(newText);
    }
}

void AMCurrentAmplifierSingleView::onMinusClicked()
{
    amplifier_->decreaseValue();
}

void AMCurrentAmplifierSingleView::onPlusClicked()
{
    amplifier_->increaseValue();
}

void AMCurrentAmplifierSingleView::onCustomContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    QAction *basic = menu.addAction("Basic view");
    basic->setDisabled(viewMode_ == AMCurrentAmplifierView::Basic);

    QAction *advanced = menu.addAction("Advanced view");
    advanced->setDisabled(viewMode_ == AMCurrentAmplifierView::Advanced);

    if (amplifier_ && amplifier_->supportsGainMode() && amplifier_->supportsSensitivityMode()) {
        menu.addSeparator();

        QAction *gain = menu.addAction("Gain view");
        gain->setDisabled(viewMode_ == AMCurrentAmplifierView::Basic || amplifier_->inGainMode());

        QAction *sensitivity = menu.addAction("Sensitivity view");
        sensitivity->setDisabled(viewMode_ == AMCurrentAmplifierView::Basic || amplifier_->inSensitivityMode());
    }

    QAction *selected = menu.exec(mapToGlobal(position));

    if (selected) {
        if (selected->text() == "Basic view")
            setViewMode(Basic);

        else if (selected->text() == "Advanced view")
            setViewMode(Advanced);

        else if (selected->text() == "Gain view")
            amplifier_->setAmplifierMode(AMCurrentAmplifier::Gain);

        else if (selected->text() == "Sensitivity view")
            amplifier_->setAmplifierMode(AMCurrentAmplifier::Sensitivity);
    }
}

QString AMCurrentAmplifierSingleView::valueToString(double value, const QString &units) const
{
    return QString("%1 %2").arg(value, 0, 'g', 2).arg(units);
}

void AMCurrentAmplifierSingleView::refreshValues()
{
    initialized_ = false;

    value_->clear();

    // (re)populate value_ with appropriate options provided by the amplifier.
    QStringList unitsList = amplifier_->unitsList();
    QList<double> valuesList = amplifier_->values();

    foreach (QString units, unitsList) {
        foreach (double value, valuesList) {
            QString item = valueToString(value, units);
            value_->addItem(item);
        }
    }

    // values displayed should represent the amplifier's current state.
    onAmplifierValueChanged();

    initialized_ = true;
}

void AMCurrentAmplifierSingleView::refreshButtons()
{
    minus_->setDisabled(amplifier_->atMinimumValue());
    plus_->setDisabled(amplifier_->atMaximumValue());
}
