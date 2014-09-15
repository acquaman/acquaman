#include "BioXASSideMonochromatorViewBasic.h"

BioXASSideMonochromatorViewBasic::BioXASSideMonochromatorViewBasic(QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    mono_ = BioXASSideBeamline::bioXAS()->mono();

    // Create UI.

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    energy_ = new QDoubleSpinBox(this);
    energy_->setSuffix("eV");
    energy_->setMinimum(0);
    energy_->setMaximum(1000000);
    layout->addWidget(energy_);

    energyFeedback_ = new QLabel(this);
    energyFeedback_->setText("---");
    layout->addWidget(energyFeedback_);

    setLayout(layout);

    // Make connections.

    connect( energy_, SIGNAL(editingFinished()), this, SLOT(onEnergyChanged()) );
}

BioXASSideMonochromatorViewBasic::~BioXASSideMonochromatorViewBasic()
{

}

BioXASSideMonochromator *BioXASSideMonochromatorViewBasic::mono() const
{
    return mono_;
}

void BioXASSideMonochromatorViewBasic::setMonochromator(BioXASSideMonochromator *mono)
{
    if (mono_ != mono) {

        if (mono_)
            disconnect( mono_, 0, this, 0 );

        mono_ = mono;

        if (mono_) {
//            connect( mono_, SIGNAL(energyChanged(double)), this, SLOT(onMonoEnergyChanged()) );
//            energy_->setValue(mono_->energy());
        }

    }
}

void BioXASSideMonochromatorViewBasic::onEnergyChanged()
{
    if (mono_) {
        // set mono energy setpoint.

        double setpoint = energy_->value();
        qDebug() << "Setpoint energy: " << setpoint;

        //    mono_->setEnergy(setpoint);
    }
}

void BioXASSideMonochromatorViewBasic::onMonoEnergyChanged()
{

}
