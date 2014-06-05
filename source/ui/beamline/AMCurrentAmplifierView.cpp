#include "AMCurrentAmplifierView.h"

AMCurrentAmplifierView::AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, QWidget *parent) :
    QWidget(parent)
{
    amplifier_ = 0;

    minus_ = new QToolButton();
    minus_->setMaximumSize(25, 25);
    minus_->setIcon(QIcon(":/22x22/list-remove.png"));
    connect( minus_, SIGNAL(clicked()), this, SLOT(onMinusClicked()) );

    plus_ = new QToolButton();
    plus_->setMaximumSize(25, 25);
    plus_->setIcon(QIcon(":/22x22/list-add.png"));
    connect( plus_, SIGNAL(clicked()), this, SLOT(onPlusClicked()) );

    value_ = new QComboBox();
    connect( value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)) );

    units_ = new QComboBox();
    connect( units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)) );

    setCurrentAmplifier(amplifier);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

    QHBoxLayout *layout = new QHBoxLayout();

    layout->addWidget(minus_);
    layout->addWidget(plus_);
    layout->addWidget(value_);
    layout->addWidget(units_);

    setLayout(layout);
}

AMCurrentAmplifierView::~AMCurrentAmplifierView()
{

}

AMCurrentAmplifier* AMCurrentAmplifierView::currentAmplifier() const
{
    return amplifier_;
}

AMCurrentAmplifier::AmplifierMode AMCurrentAmplifierView::viewMode() const
{
    return viewMode_;
}

void AMCurrentAmplifierView::setCurrentAmplifier(AMCurrentAmplifier *amplifier)
{
    if (amplifier_) {
        disconnect( amplifier_, SIGNAL(amplifierModeChanged(AMCurrentAmplifier::AmplifierMode)), this, SLOT(setViewMode(AMCurrentAmplifier::AmplifierMode)) );
        disconnect( amplifier_, SIGNAL(unitsChanged(int)), units_, SLOT(setCurrentIndex(int)) );


        if (viewMode_ == AMCurrentAmplifier::Gain) {
            disconnect( amplifier_, SIGNAL(gainChanged(int)), value_, SLOT(setCurrentIndex(int)) );
            disconnect( amplifier_, SIGNAL(minimumGain(bool)), minus_, SLOT(setDisabled(bool)) );
            disconnect( amplifier_, SIGNAL(maximumGain(bool)), plus_, SLOT(setDisabled(bool)) );

        } else if (viewMode_ == AMCurrentAmplifier::Sensitivity) {
            disconnect( amplifier_, SIGNAL(sensitivityChanged(int)), value_, SLOT(setCurrentIndex(int)) );
            disconnect( amplifier_, SIGNAL(minimumSensitivity(bool)), minus_, SLOT(setDisabled(bool)) );
            disconnect( amplifier_, SIGNAL(maximumSensitivity(bool)), plus_, SLOT(setDisabled(bool)) );
        }
    }

    amplifier_ = amplifier;
    emit currentAmplifierChanged(amplifier_);

    if (amplifier_) {
        setViewMode(amplifier_->amplifierMode());

        connect( amplifier_, SIGNAL(amplifierModeChanged(AMCurrentAmplifier::AmplifierMode)), this, SLOT(setViewMode(AMCurrentAmplifier::AmplifierMode)) );
        connect( amplifier_, SIGNAL(unitsChanged(int)), units_, SLOT(setCurrentIndex(int)) );

        if (viewMode_ == AMCurrentAmplifier::Gain) {
            connect( amplifier_, SIGNAL(gainChanged(int)), value_, SLOT(setCurrentIndex(int)) );
            connect( amplifier_, SIGNAL(minimumGain(bool)), minus_, SLOT(setDisabled(bool)) );
            connect( amplifier_, SIGNAL(maximumGain(bool)), plus_, SLOT(setDisabled(bool)) );

        } else if (viewMode_ == AMCurrentAmplifier::Sensitivity) {
            connect( amplifier_, SIGNAL(sensitivityChanged(int)), value_, SLOT(setCurrentIndex(int)) );
            connect( amplifier_, SIGNAL(minimumSensitivity(bool)), minus_, SLOT(setDisabled(bool)) );
            connect( amplifier_, SIGNAL(maximumSensitivity(bool)), plus_, SLOT(setDisabled(bool)) );
        }
    }

    refreshButtons();
    refreshDisplayValues();
}

void AMCurrentAmplifierView::setViewMode(AMCurrentAmplifier::AmplifierMode newMode)
{
    viewMode_ = newMode;

    refreshButtons();
    refreshDisplayValues();
}

void AMCurrentAmplifierView::refreshDisplayValues()
{
    value_->clear();
    units_->clear();

    // repopulate value_ and units_ with appropriate options from the amplifier.
    value_->addItems(amplifier_->valuesList());
    units_->addItems(amplifier_->unitsList());
}

void AMCurrentAmplifierView::refreshButtons()
{
    if (!amplifier_) {
        plus_->setDisabled(true);
        minus_->setDisabled(true);

    } else if (viewMode_ == AMCurrentAmplifier::Gain) {

        if (amplifier_->atMinimumGain())
            minus_->setDisabled(true);

        if (amplifier_->atMaximumGain())
            plus_->setDisabled(true);

    } else if (viewMode_ == AMCurrentAmplifier::Sensitivity) {

        if (amplifier_->atMinimumSensitivity())
            minus_->setDisabled(true);

        if (amplifier_->atMaximumSensitivity())
            plus_->setDisabled(true);
    }
}

void AMCurrentAmplifierView::onValueComboBoxChanged(int index)
{
    if (amplifier_)
        amplifier_->setValue(QString::number(index));
}

void AMCurrentAmplifierView::onUnitsComboBoxChanged(int index)
{
    Q_UNUSED(index)
}

void AMCurrentAmplifierView::onMinusClicked()
{
    if (amplifier_ && viewMode_ == AMCurrentAmplifier::Gain) {
        qDebug() << "Gain, minus clicked.";
        amplifier_->decreaseGain();

    } else if (amplifier_ && viewMode_ == AMCurrentAmplifier::Sensitivity) {
        amplifier_->decreaseSensitivity();
    }
}

void AMCurrentAmplifierView::onPlusClicked()
{
    if (amplifier_ && viewMode_ == AMCurrentAmplifier::Gain) {
        amplifier_->increaseGain();

    } else if (amplifier_ && viewMode_ == AMCurrentAmplifier::Sensitivity) {
        amplifier_->increaseSensitivity();

    }
}

void AMCurrentAmplifierView::onCustomContextMenuRequested(QPoint position)
{
    // only show the menu if there is a valid amplifier and that amplifier supports multiple view options.
    if (amplifier_ && amplifier_->supportsGainMode() && amplifier_->supportsSensitivityMode()) {
        QMenu menu(this);

        QAction *gain = menu.addAction("Gain view");
        gain->setDisabled(viewMode_ == AMCurrentAmplifier::Gain);

        QAction *sensitivity = menu.addAction("Sensitivity view");
        sensitivity->setDisabled(viewMode_ == AMCurrentAmplifier::Sensitivity);

        QAction *selected = menu.exec(mapToGlobal(position));

        if (selected) {
            if (selected->text() == "Gain view")
                amplifier_->setAmplifierMode(AMCurrentAmplifier::Gain);

            else if (selected->text() == "Sensitivity view")
                amplifier_->setAmplifierMode(AMCurrentAmplifier::Sensitivity);
        }
    }
}
