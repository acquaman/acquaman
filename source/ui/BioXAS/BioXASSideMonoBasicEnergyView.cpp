#include "BioXASSideMonoBasicEnergyView.h"

BioXASSideMonoBasicEnergyView::BioXASSideMonoBasicEnergyView(BioXASSideMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
    // Set variables.

    mono_ = 0;

    // Create UI.

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);

    energySetpoint_ = new QDoubleSpinBox(this);
    energySetpoint_->setSuffix(" eV");
    energySetpoint_->setMinimum(0);
    energySetpoint_->setMaximum(1000000);
    layout->addWidget(energySetpoint_);

    energy_ = new QLabel(this);
    layout->addWidget(energy_);

    setLayout(layout);


    // Initial conditions.

    energySetpoint_->setValue(0);
    energy_->setText("--- eV");

    // Current conditions.

    setMono(mono);

    connect( energy_, SIGNAL(editingFinished()), this, SLOT(onEnergyChanged()) );
}

BioXASSideMonoBasicEnergyView::~BioXASSideMonoBasicEnergyView()
{

}

BioXASSideMonochromator* BioXASSideMonoBasicEnergyView::mono() const
{
    return mono_;
}

void BioXASSideMonoBasicEnergyView::setMono(BioXASSideMonochromator *mono)
{
    if (mono_ != mono) {

        if (mono_)
            disconnect( mono_, 0, this, 0 );

        mono_ = mono;

        if (mono_) {
            connect( mono_, SIGNAL(connected(bool)), this, SLOT(onMonoConnectedChanged()) );
            connect( mono_, SIGNAL(energyChanged(double)), this, SLOT(onMonoEnergyChanged(double)) );

            energySetpoint_->setValue(mono_->energy());
            onMonoConnectedChanged();
        }

    }
}

void BioXASSideMonoBasicEnergyView::onEnergyChanged()
{
    if (mono_ && mono_->isConnected()) {
        mono_->setEnergy(energySetpoint_->value());
    }
}

void BioXASSideMonoBasicEnergyView::onMonoConnectedChanged()
{
    if (mono_ && !mono_->isConnected())
        energy_->setText("--- eV");
}

void BioXASSideMonoBasicEnergyView::onMonoEnergyChanged(double newEnergy)
{
    energy_->setText(QString::number(newEnergy) + " eV");
}
