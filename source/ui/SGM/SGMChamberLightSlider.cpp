#include "SGMChamberLightSlider.h"
#include "beamline/AMControl.h"
SGMChamberLightSlider::SGMChamberLightSlider(AMControl* chamberLight, QWidget *parent) :
    QSlider(parent)
{
    connectedOnce_ = false;
    chamberLight_ = chamberLight;

    if(chamberLight_) {
        connect(chamberLight_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));

        if(chamberLight_->isConnected()) {
            onControlConnected(true);
        }
    }

}

SGMChamberLightSlider::SGMChamberLightSlider(AMControl* chamberLight, Qt::Orientation orientation, QWidget *parent) :
    QSlider(orientation, parent)
{
    connectedOnce_ = false;
    chamberLight_ = chamberLight;

    connect(chamberLight_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));

}

void SGMChamberLightSlider::onControlConnected(bool isConnected)
{
    setEnabled(isConnected);

    if(!connectedOnce_) {

        connectedOnce_ = true;
        setupData();
        setupConnections();
    }
}

void SGMChamberLightSlider::onControlMovingChanged(bool isMoving)
{
    setEnabled(!isMoving);
}

void SGMChamberLightSlider::onControlValueChanged(double value)
{
    setValue(int(value));
}

void SGMChamberLightSlider::onSliderReleased()
{
    chamberLight_->move(double(value()));
}

void SGMChamberLightSlider::setupData()
{
    setValue(int(chamberLight_->value()));
    setRange(int(chamberLight_->minimumValue()), int(chamberLight_->maximumValue()));
    setEnabled(chamberLight_->isConnected());
}

void SGMChamberLightSlider::setupConnections()
{
    connect(chamberLight_, SIGNAL(movingChanged(bool)), this, SLOT(onControlMovingChanged(bool)));
    connect(chamberLight_, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged(double)));
    connect(this, SIGNAL(sliderReleased()), this, SLOT(onSliderReleased()));
}


