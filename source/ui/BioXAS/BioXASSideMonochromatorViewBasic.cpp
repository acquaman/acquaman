#include "BioXASSideMonochromatorViewBasic.h"

BioXASSideMonochromatorViewBasic::BioXASSideMonochromatorViewBasic(QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    mono_ = BioXASSideBeamline::bioXAS()->mono();

    // Create UI.

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);

    energy_ = new QDoubleSpinBox(this);
    energy_->setSuffix(" eV");
    energy_->setMinimum(0);
    energy_->setMaximum(1000000);
    mainLayout->addWidget(energy_);

    energyFeedback_ = new QLabel(this);
    energyFeedback_->setText("---");
    mainLayout->addWidget(energyFeedback_);

    setLayout(mainLayout);

    // Make connections.

    connect( energy_, SIGNAL(editingFinished()), this, SLOT(onEnergyChanged()) );
//    connect( mono_, SIGNAL(energyChanged(double)), this, SLOT(onMonoEnergyChanged()) );
}

BioXASSideMonochromatorViewBasic::~BioXASSideMonochromatorViewBasic()
{

}

BioXASSideMonochromator *BioXASSideMonochromatorViewBasic::mono() const
{
    return mono_;
}

void BioXASSideMonochromatorViewBasic::onEnergyChanged()
{
    // set mono energy setpoint.
}

void BioXASSideMonochromatorViewBasic::onMonoEnergyChanged()
{
    // set energy feedback value.
}
