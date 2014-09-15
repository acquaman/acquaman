#include "BioXASSideMonochromatorCrystalChangeView.h"

BioXASSideMonochromatorCrystalChangeView::BioXASSideMonochromatorCrystalChangeView(BioXASSideMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    mono_ = 0;

    // Create super simple UI.

    QGridLayout *layout = new QGridLayout();

    QLabel *brakeStatusLabel = new QLabel("Brake status: ", this);
    brakeStatus_ = new QLabel(this);
    brakeStatusLabel->setBuddy(brakeStatus_);
    layout->addWidget(brakeStatusLabel, 0, 0);
    layout->addWidget(brakeStatus_, 0, 1);

    QLabel *positionStatusLabel = new QLabel("Crystal change position: ", this);
    positionStatus_ = new QLabel(this);
    positionStatusLabel->setBuddy(positionStatus_);
    layout->addWidget(positionStatusLabel, 1, 0);
    layout->addWidget(positionStatus_, 1, 1);

    QLabel *keyStatusLabel = new QLabel("Key status: ", this);
    keyStatus_ = new QLabel(this);
    keyStatusLabel->setBuddy(keyStatus_);
    layout->addWidget(keyStatusLabel, 2, 0);
    layout->addWidget(keyStatus_, 2, 1);

    setLayout(layout);

    // Initial settings.

    brakeStatus_->setText("---");
    positionStatus_->setText("---");
    keyStatus_->setText("---");

    // Current settings.

    setMonochromator(mono);
}

BioXASSideMonochromatorCrystalChangeView::~BioXASSideMonochromatorCrystalChangeView()
{

}

void BioXASSideMonochromatorCrystalChangeView::setMonochromator(BioXASSideMonochromator *mono)
{
    if (mono_ != mono) {

        if (mono_) {
            disconnect( mono_, 0, this, 0 );
        }

        mono_ = mono;

        if (mono_) {
            onMonoBrakeStatusChanged();
            onMonoCrystalChangePositionStatusChanged();
            onMonoKeyStatusChanged();

            connect( mono_, SIGNAL(brakeOffStatusChanged(bool)), this, SLOT(onMonoBrakeStatusChanged()) );
            connect( mono_, SIGNAL(crystalChangePositionStatusChanged(bool)), this, SLOT(onMonoCrystalChangePositionStatusChanged()) );
            connect( mono_, SIGNAL(keyStatusChanged(bool)), this, SLOT(onMonoKeyStatusChanged()) );
        }
    }
}

void BioXASSideMonochromatorCrystalChangeView::setBrakeStatus(bool on)
{
    if (on)
        brakeStatus_->setText("On");
    else
        brakeStatus_->setText("Off");
}

void BioXASSideMonochromatorCrystalChangeView::setPositionStatus(bool inPosition)
{
    if (inPosition)
        positionStatus_->setText("Yes");
    else
        positionStatus_->setText("No");
}

void BioXASSideMonochromatorCrystalChangeView::setKeyStatus(bool turned)
{
    if (turned)
        keyStatus_->setText("Turned");
    else
        keyStatus_->setText("Not turned");
}

void BioXASSideMonochromatorCrystalChangeView::onMonoBrakeStatusChanged()
{
    if (mono_)
        setBrakeStatus(!mono_->brakeOff());
}

void BioXASSideMonochromatorCrystalChangeView::onMonoCrystalChangePositionStatusChanged()
{
    if (mono_)
        setPositionStatus(mono_->crystalChangePosition());
}

void BioXASSideMonochromatorCrystalChangeView::onMonoKeyStatusChanged()
{
    if (mono_)
        setKeyStatus(mono_->keyStatus());
}
