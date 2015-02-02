#include "BioXASMainMonochromatorCrystalChangeView.h"
#include <QDebug>

BioXASMainMonochromatorCrystalChangeView::BioXASMainMonochromatorCrystalChangeView(BioXASMainMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
    // Initialize variables.

    mono_ = 0;

    // Create UI elements.

    QLabel *slitsClosedPrompt = new QLabel("Slits status:");
    slitsClosed_ = new QLabel();

    QLabel *paddleOutPrompt = new QLabel("Paddle status:");
    paddleOut_ = new QLabel();

    QLabel *keyStatusPrompt = new QLabel("Key status:");
    keyStatus_ = new QLabel();

    QLabel *brakeStatusPrompt = new QLabel("Brake status:");
    brakeStatus_ = new QLabel();

    braggMotorPosition_ = new AMExtendedControlEditor(0, 0, true);
    braggMotorPosition_->overrideTitle("Bragg motor position");
//    braggMotorPosition_->setReadOnly(true);
//    braggMotorPosition_->setControlFormat('f', 2);
    braggMotorPosition_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    crystalChangeMotorPosition_ = new AMExtendedControlEditor(0, 0, true);
    crystalChangeMotorPosition_->overrideTitle("Crystal change motor position");
//    crystalChangeMotorPosition_->setReadOnly(true);
//    crystalChangeMotorPosition_->setControlFormat('f', 2);
    crystalChangeMotorPosition_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QGroupBox *monoControls = new QGroupBox("Monochromator Controls");

    // Create and set layouts.

    QGridLayout *statusLayout = new QGridLayout();
    statusLayout->setMargin(0);
    statusLayout->addWidget(slitsClosedPrompt, 0, 0);
    statusLayout->addWidget(slitsClosed_, 0, 1);
    statusLayout->addWidget(paddleOutPrompt, 1, 0);
    statusLayout->addWidget(paddleOut_, 1, 1);
    statusLayout->addWidget(keyStatusPrompt, 2, 0);
    statusLayout->addWidget(keyStatus_, 2, 1);
    statusLayout->addWidget(brakeStatusPrompt, 3, 0);
    statusLayout->addWidget(brakeStatus_, 3, 1);

    QVBoxLayout *motorsLayout = new QVBoxLayout();
    motorsLayout->setMargin(0);
    motorsLayout->addWidget(braggMotorPosition_);
    motorsLayout->addWidget(crystalChangeMotorPosition_);

    QVBoxLayout *monoControlsLayout = new QVBoxLayout();
    monoControlsLayout->addLayout(statusLayout);
    monoControlsLayout->addLayout(motorsLayout);
    monoControls->setLayout(monoControlsLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(monoControls);

    setLayout(layout);

    // Current settings.

    setMono(mono);
}

BioXASMainMonochromatorCrystalChangeView::~BioXASMainMonochromatorCrystalChangeView()
{

}

void BioXASMainMonochromatorCrystalChangeView::setMono(BioXASMainMonochromator *newMono)
{
    if (mono_ != newMono)
        emit monoChanged(mono_ = newMono);
}
