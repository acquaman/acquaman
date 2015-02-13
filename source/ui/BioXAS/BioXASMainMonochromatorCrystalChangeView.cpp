#include "BioXASMainMonochromatorCrystalChangeView.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeView::BioXASMainMonochromatorCrystalChangeView(BioXASMainMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
    // Initialize variables.

    mono_ = 0;
    disconnectedText_ = "---";

    // Create UI elements.

    QGroupBox *monoControls = new QGroupBox("Crystal change status");

    QLabel *regionPrompt = new QLabel("Region:");
    region_ = new QLabel();

    QLabel *slitsClosedPrompt = new QLabel("Slits:");
    slitsClosed_ = new QLabel();

    QLabel *paddleOutPrompt = new QLabel("Paddle:");
    paddleOut_ = new QLabel();

    QLabel *keyStatusPrompt = new QLabel("Key:");
    keyStatus_ = new QLabel();

    QLabel *brakeStatusPrompt = new QLabel("Brake:");
    brakeStatus_ = new QLabel();

    QLabel *braggMotorPositionPrompt = new QLabel("Bragg motor:");
    braggMotorPosition_ = new QLabel();
    braggMotorAtCrystalChangePosition_ = new QLabel();

    QLabel *crystalChangeMotorPositionPrompt = new QLabel("CC motor:");
    crystalChangeMotorCWLimit_ = new QLabel();
    crystalChangeMotorCCWLimit_ = new QLabel();

    // Create and set layouts.

    QGridLayout *statusLayout = new QGridLayout();
    statusLayout->addWidget(regionPrompt, 0, 0);
    statusLayout->addWidget(region_, 0, 1);
    statusLayout->addWidget(slitsClosedPrompt, 1, 0);
    statusLayout->addWidget(slitsClosed_, 1, 1);
    statusLayout->addWidget(paddleOutPrompt, 2, 0);
    statusLayout->addWidget(paddleOut_, 2, 1);
    statusLayout->addWidget(keyStatusPrompt, 3, 0);
    statusLayout->addWidget(keyStatus_, 3, 1);
    statusLayout->addWidget(braggMotorPositionPrompt, 4, 0);
    statusLayout->addWidget(braggMotorPosition_, 4, 1);
    statusLayout->addWidget(braggMotorAtCrystalChangePosition_, 5, 1);
    statusLayout->addWidget(brakeStatusPrompt, 6, 0);
    statusLayout->addWidget(brakeStatus_, 6, 1);
    statusLayout->addWidget(crystalChangeMotorPositionPrompt, 7, 0);
    statusLayout->addWidget(crystalChangeMotorCWLimit_, 7, 1);
    statusLayout->addWidget(crystalChangeMotorCCWLimit_, 8, 1);

    monoControls->setLayout(statusLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->addWidget(monoControls);

    setLayout(layout);

    // Initial settings.

    updateView();

    // Current settings.

    setMono(mono);
}

BioXASMainMonochromatorCrystalChangeView::~BioXASMainMonochromatorCrystalChangeView()
{

}

void BioXASMainMonochromatorCrystalChangeView::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono) {

        if (mono_)
            disconnect( mono_, 0, this, 0 );

        mono_ = newMono;

        if (mono_) {
            connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConnectedChanged()) );

            connect( mono_, SIGNAL(regionChanged(BioXASMainMonochromator::Region)), this, SLOT(onRegionStatusChanged()) );
            connect( mono_, SIGNAL(slitsClosedStatusChanged(bool)), this, SLOT(onSlitsClosedChanged()) );
            connect( mono_, SIGNAL(paddleOutStatusChanged(bool)), this, SLOT(onPaddleOutChanged()) );
            connect( mono_, SIGNAL(keyEnabledStatusChanged(bool)), this, SLOT(onKeyStatusChanged()) );
            connect( mono_, SIGNAL(braggMotorPositionChanged(double)), this, SLOT(onBraggMotorPositionChanged()) );
            connect( mono_, SIGNAL(braggMotorAtCrystalChangePositionStatusChanged(bool)), this, SLOT(onBraggMotorAtCrystalChangePositionChanged()) );
            connect( mono_, SIGNAL(crystalChangeBrakeEnabledChanged(bool)), this, SLOT(onBrakeStatusChanged()) );
            connect( mono_, SIGNAL(crystalChangeMotorCWLimitStatusChanged(bool)), this, SLOT(onCrystalChangeMotorCWLimitStatusChanged()) );
            connect( mono_, SIGNAL(crystalChangeMotorCCWLimitStatusChanged(bool)), this, SLOT(onCrystalChangeMotorCCWLimitStatusChanged()) );

        }

        onMonoConnectedChanged();

        emit monoChanged(mono_);
    }
}

void BioXASMainMonochromatorCrystalChangeView::updateView()
{
    onRegionStatusChanged();
    onSlitsClosedChanged();
    onPaddleOutChanged();
    onKeyStatusChanged();
    onBraggMotorPositionChanged();
    onBraggMotorAtCrystalChangePositionChanged();
    onBrakeStatusChanged();
    onCrystalChangeMotorCWLimitStatusChanged();
    onCrystalChangeMotorCCWLimitStatusChanged();
}

void BioXASMainMonochromatorCrystalChangeView::onMonoConnectedChanged()
{
    updateView();
}

void BioXASMainMonochromatorCrystalChangeView::onRegionStatusChanged()
{
    QString text;

    if (mono_) {
        if (mono_->region() == BioXASMainMonochromator::A) {
            text = "A";

        } else if (mono_->region() == BioXASMainMonochromator::B) {
            text = "B";

        } else {
            text = "None";
        }

    } else {
        text = disconnectedText_;
    }

    region_->setText(text);
}

void BioXASMainMonochromatorCrystalChangeView::onSlitsClosedChanged()
{
    QString text;

    if (mono_ && mono_->slitsClosedControl() && mono_->slitsClosedControl()->isConnected()) {
        // if the mono and slits closed control are connected, the text accurately reflects open/close state.

        if (mono_->slitsClosed())
            text = "Closed";
        else
            text = "Open";

    } else {
        // if either the mono or the slits closed control are disconnected, value shown is "---".
        text = disconnectedText_;
    }

    slitsClosed_->setText(text);
}

void BioXASMainMonochromatorCrystalChangeView::onPaddleOutChanged()
{
    QString text;

    if (mono_ && mono_->phosphorPaddleOutControl() && mono_->phosphorPaddleOutControl()->isConnected()) {
        if (mono_->paddleOut())
            text = "Out";
        else
            text = "Not out";

    } else {
        text = disconnectedText_;
    }

    paddleOut_->setText(text);
}

void BioXASMainMonochromatorCrystalChangeView::onKeyStatusChanged()
{
    QString text;

    if (mono_ && mono_->keyStatusControl() && mono_->keyStatusControl()->isConnected()) {
        if (mono_->keyStatusEnabled())
            text = "Enabled";
        else
            text = "Disabled";

    } else {
        text = disconnectedText_;
    }

    keyStatus_->setText(text);
}

void BioXASMainMonochromatorCrystalChangeView::onBraggMotorPositionChanged()
{
    QString newPosition;

    if (mono_ && mono_->braggMotor() && mono_->braggMotor()->isConnected())
        newPosition = QString::number(mono_->braggMotor()->value(), 'f', 4);
    else
        newPosition = disconnectedText_;

    braggMotorPosition_->setText(newPosition);
}

void BioXASMainMonochromatorCrystalChangeView::onBraggMotorAtCrystalChangePositionChanged()
{
    qDebug() << "Update bragg motor crystal change position status display.";

    QString text;

    if (mono_) {
        if (mono_->braggMotorAtCrystalChangePositionStatusControl() && mono_->braggMotorAtCrystalChangePositionStatusControl()->isConnected() && mono_->atCrystalChangePosition())
            text = "In position";
        else
            text = "NOT in position";
    } else {
        text = disconnectedText_;
    }

    braggMotorAtCrystalChangePosition_->setText(text);
}

void BioXASMainMonochromatorCrystalChangeView::onBrakeStatusChanged()
{
    QString text;

    if (mono_ && mono_->brakeStatusControl() && mono_->brakeStatusControl()->isConnected()) {
        if (mono_->brakeStatusEnabled() == 0)
            text = "Enabled";
        else
            text = "Disabled";

    } else {
        text = disconnectedText_;
    }

    brakeStatus_->setText(text);
}

void BioXASMainMonochromatorCrystalChangeView::onCrystalChangeMotorCWLimitStatusChanged()
{
    QString text;

    if (mono_ && mono_->crystalChangeMotorCWLimitStatusControl() && mono_->crystalChangeMotorCWLimitStatusControl()->isConnected()) {
        if (mono_->crystalChangeMotorAtCWLimit())
            text = "At CW limit";
        else
            text = "NOT at CW limit";

    } else {
        text = disconnectedText_;
    }

    crystalChangeMotorCWLimit_->setText(text);
}

void BioXASMainMonochromatorCrystalChangeView::onCrystalChangeMotorCCWLimitStatusChanged()
{
    QString text;

    if (mono_ && mono_->crystalChangeMotorCCWLimitStatusControl() && mono_->crystalChangeMotorCCWLimitStatusControl()->isConnected()) {
        if (mono_->crystalChangeMotorAtCCWLimit())
            text = "At CCW limit";
        else
            text = "NOT at CCW limit";

    } else {
        text = disconnectedText_;
    }

    crystalChangeMotorCCWLimit_->setText(text);
}
