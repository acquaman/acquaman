#include "AMCurrentAmplifierView.h"

AMCurrentAmplifierView::AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, bool showName, QWidget *parent) :
    QWidget(parent)
{
    initialized_ = false;

    amplifier_ = amplifier;
    connect( amplifier_, SIGNAL(amplifierModeChanged(AMCurrentAmplifier::AmplifierMode)), this, SLOT(refreshView()) );

    QLabel *name = new QLabel(amplifier_->name());

    minus_ = new QToolButton();
    minus_->setMaximumSize(25, 25);
    minus_->setIcon(QIcon(":/22x22/list-remove.png"));
    connect( minus_, SIGNAL(clicked()), this, SLOT(onMinusClicked()) );
    connect( amplifier_, SIGNAL(minimumGain(bool)), minus_, SLOT(setDisabled(bool)) );
    connect( amplifier_, SIGNAL(minimumSensitivity(bool)), minus_, SLOT(setDisabled(bool)) );

    plus_ = new QToolButton();
    plus_->setMaximumSize(25, 25);
    plus_->setIcon(QIcon(":/22x22/list-add.png"));
    connect( plus_, SIGNAL(clicked()), this, SLOT(onPlusClicked()) );
    connect( amplifier_, SIGNAL(maximumGain(bool)), plus_, SLOT(setDisabled(bool)) );
    connect( amplifier_, SIGNAL(maximumSensitivity(bool)), plus_, SLOT(setDisabled(bool)) );

    value_ = new QComboBox();
    connect( value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)) );
    connect( amplifier_, SIGNAL(gainChanged(int)), this, SLOT(onAmplifierValueChanged(int)) );
    connect( amplifier_, SIGNAL(sensitivityChanged(int)), this, SLOT(onAmplifierValueChanged(int)) );

    units_ = new QComboBox();
//    connect( units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)) );
    connect( amplifier_, SIGNAL(unitsIndexChanged(int)), this, SLOT(onAmplifierUnitsChanged(int)) );

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

    refreshView();

    QHBoxLayout *layout = new QHBoxLayout();

    if (showName)
        layout->addWidget(name);
    layout->addWidget(minus_);
    layout->addWidget(plus_);
    layout->addWidget(value_);
    layout->addWidget(units_);

    setLayout(layout);

    initialized_ = true;
}

AMCurrentAmplifierView::~AMCurrentAmplifierView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierView::currentAmplifier() const
{
    return amplifier_;
}

bool AMCurrentAmplifierView::initialized() const
{
    return initialized_;
}

void AMCurrentAmplifierView::refreshView()
{
    refreshDisplayValues();
    refreshButtons();
}

void AMCurrentAmplifierView::onValueComboBoxChanged(int index)
{
    // the initialized_ boolean prevents the display from setting the amplifier value while initializing <- undesirable behavior.
    if (initialized_) {
        amplifier_->setValue( QString::number(index) + " " + QString::number(units_->currentIndex()) );
    }
}

void AMCurrentAmplifierView::onUnitsComboBoxChanged(int index)
{
    if (initialized_) {
        amplifier_->setValue( QString::number(value_->currentIndex()) + " " + QString::number(index) );
    }
}

void AMCurrentAmplifierView::onAmplifierValueChanged(int valueIndex)
{
    amplifier_->blockSignals(true);
    value_->setCurrentIndex(valueIndex);
    amplifier_->blockSignals(false);
}

void AMCurrentAmplifierView::onAmplifierUnitsChanged(int unitsIndex)
{
    amplifier_->blockSignals(true);
    qDebug() << "Set units combo box : " << unitsIndex;
    units_->setCurrentIndex(unitsIndex);
    amplifier_->blockSignals(false);
}

void AMCurrentAmplifierView::onMinusClicked()
{
    if (amplifier_->amplifierMode() == AMCurrentAmplifier::Gain) {
        amplifier_->decreaseGain();

    } else if (amplifier_->amplifierMode() == AMCurrentAmplifier::Sensitivity) {
        amplifier_->decreaseSensitivity();
    }
}

void AMCurrentAmplifierView::onPlusClicked()
{
    if (amplifier_->amplifierMode() == AMCurrentAmplifier::Gain) {
        amplifier_->increaseGain();

    } else if (amplifier_->amplifierMode() == AMCurrentAmplifier::Sensitivity) {
        amplifier_->increaseSensitivity();
    }
}

void AMCurrentAmplifierView::onCustomContextMenuRequested(QPoint position)
{
    // only show the menu if there is a valid amplifier and that amplifier supports multiple view options.
    if (amplifier_ && amplifier_->supportsGainMode() && amplifier_->supportsSensitivityMode()) {
        QMenu menu(this);

        QAction *gain = menu.addAction("Gain view");
        gain->setDisabled(amplifier_->amplifierMode() == AMCurrentAmplifier::Gain);

        QAction *sensitivity = menu.addAction("Sensitivity view");
        sensitivity->setDisabled(amplifier_->amplifierMode() == AMCurrentAmplifier::Sensitivity);

        QAction *selected = menu.exec(mapToGlobal(position));

        if (selected) {
            if (selected->text() == "Gain view")
                amplifier_->setAmplifierMode(AMCurrentAmplifier::Gain);

            else if (selected->text() == "Sensitivity view")
                amplifier_->setAmplifierMode(AMCurrentAmplifier::Sensitivity);
        }
    }
}

void AMCurrentAmplifierView::refreshDisplayValues()
{
    value_->clear();
    units_->clear();

    // repopulate value_ and units_ with appropriate options provided by the amplifier.
    // the values displayed should represent the amplifier's current state.

    value_->addItems(amplifier_->valuesList());
    onAmplifierValueChanged(amplifier_->valueIndex());

    units_->addItems(amplifier_->unitsList());
}

void AMCurrentAmplifierView::refreshButtons()
{
    if (amplifier_->amplifierMode() == AMCurrentAmplifier::Gain) {
        minus_->setDisabled(amplifier_->atMinimumGain() == true);
        plus_->setDisabled(amplifier_->atMaximumGain() == true);

    } else if (amplifier_->amplifierMode() == AMCurrentAmplifier::Sensitivity) {
        minus_->setDisabled(amplifier_->atMinimumSensitivity() == true);
        plus_->setDisabled(amplifier_->atMaximumSensitivity() == true);
    }
}
