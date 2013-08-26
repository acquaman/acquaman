#include "AMSampleCameraWizardSelector.h"

#include <QFrame>
#include <QLayout>
#include <QPushButton>
#include <QDebug>


AMSampleCameraWizardSelector::AMSampleCameraWizardSelector(QWidget* parent)
    : QWidget(parent)
{

    beamWizardComplete_ = false;
    cameraWizardComplete_ = false;
    samplePlateWizardComplete_ = false;

    QVBoxLayout* sampleCameraWizardSelectorLayout = new QVBoxLayout();
    QFrame* beamWizardFrame = new QFrame();
    QHBoxLayout* beamWizardLayout = new QHBoxLayout();
    QFrame* cameraWizardFrame = new QFrame();
    QHBoxLayout* cameraWizardLayout = new QHBoxLayout();
    QFrame* samplePlateWizardFrame = new QFrame();
    QHBoxLayout* samplePlateWizardLayout = new QHBoxLayout();

    sampleCameraWizardSelectorLayout->setContentsMargins(0,0,0,0);
    beamWizardLayout->setContentsMargins(0,0,0,0);
    cameraWizardLayout->setContentsMargins(0,0,0,0);
    samplePlateWizardLayout->setContentsMargins(0,0,0,0);

    beamWizardLayout->addWidget(beamWizardButton_ = new QPushButton("Beam Wizard"));
    beamWizardLayout->addWidget(loadBeamConfigurationButton_ = new QPushButton("Load from database"));
    beamWizardLayout->addStretch();
    beamWizardFrame->setLayout(beamWizardLayout);

    cameraWizardLayout->addWidget(cameraWizardButton_ = new QPushButton("Camera Wizard"));
    cameraWizardLayout->addWidget(loadCameraConfigurationButton_ = new QPushButton("Load from database"));
    cameraWizardLayout->addStretch();
    cameraWizardFrame->setLayout(cameraWizardLayout);

    samplePlateWizardLayout->addWidget(samplePlateWizardButton_ = new QPushButton("Sample Plate Wizard"));
    samplePlateWizardLayout->addWidget(loadSamplePlateButton_ = new QPushButton("Load from database"));
    samplePlateWizardLayout->addStretch();
    samplePlateWizardFrame->setLayout(samplePlateWizardLayout);


    sampleCameraWizardSelectorLayout->addWidget(cameraWizardFrame);
    sampleCameraWizardSelectorLayout->addWidget(beamWizardFrame);
    sampleCameraWizardSelectorLayout->addWidget(samplePlateWizardFrame);
    sampleCameraWizardSelectorLayout->addStretch();
    setLayout(sampleCameraWizardSelectorLayout);

    beamWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
    cameraWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
    samplePlateWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));

    connect(beamWizardButton_, SIGNAL(clicked()), this, SIGNAL(beamWizardPressed()));
    connect(cameraWizardButton_, SIGNAL(clicked()), this, SIGNAL(cameraWizardPressed()));
    connect(samplePlateWizardButton_, SIGNAL(clicked()), this, SIGNAL(samplePlateWizardPressed()));

    connect(loadBeamConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadBeamConfiguration()));
    connect(loadCameraConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadCameraConfiguration()));
    connect(loadSamplePlateButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadSamplePlate()));

    connect(this, SIGNAL(beamCompleteChanged()), this, SLOT(onBeamCompleteChanged()));
    connect(this, SIGNAL(cameraCompleteChanged()), this, SLOT(onCameraCompleteChanged()));
    connect(this, SIGNAL(samplePlateCompleteChanged()), this, SLOT(onSamplePlateCompleteChanged()));


}

bool AMSampleCameraWizardSelector::beamWizardComplete() const
{
    return beamWizardComplete_;
}

bool AMSampleCameraWizardSelector::cameraWizardComplete() const
{
    return cameraWizardComplete_;
}

bool AMSampleCameraWizardSelector::samplePlateWizardComplete() const
{
    return samplePlateWizardComplete_;
}

void AMSampleCameraWizardSelector::setBeamWizardComplete(const bool &complete)
{
    if(complete != beamWizardComplete_)
    {
        beamWizardComplete_ = complete;
        emit beamCompleteChanged();
    }
}

void AMSampleCameraWizardSelector::setCameraWizardComplete(const bool &complete)
{
    if(complete != cameraWizardComplete_)
    {
        cameraWizardComplete_ = complete;
        emit cameraCompleteChanged();
    }
}

void AMSampleCameraWizardSelector::setSamplePlateWizardComplete(const bool &complete)
{
    if(complete != samplePlateWizardComplete_)
    {
        samplePlateWizardComplete_ = complete;
        emit samplePlateCompleteChanged();
    }
}

void AMSampleCameraWizardSelector::onBeamWizardFinished()
{
    setBeamWizardComplete(true);
}

void AMSampleCameraWizardSelector::onCameraWizardFinished()
{
    qDebug()<<"AMSampleCameraWizardSelector::onCameraWizardFinished";
    setCameraWizardComplete(true);
}

void AMSampleCameraWizardSelector::onSamplePlateWizardFinished()
{
    setSamplePlateWizardComplete(true);
}

void AMSampleCameraWizardSelector::onBeamCompleteChanged()
{

    if(beamWizardComplete())
    {
        beamWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));
    }
    else
    {
        beamWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
    }
}

void AMSampleCameraWizardSelector::onCameraCompleteChanged()
{
    qDebug()<<"AMSampleCameraWizardSelector::onCameraCompleteChanged";
    if(cameraWizardComplete())
    {
        cameraWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));
    }
    else
    {
        cameraWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
    }
}

void AMSampleCameraWizardSelector::onSamplePlateCompleteChanged()
{
    if(samplePlateWizardComplete())
    {
        samplePlateWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));
    }
    else
    {
        samplePlateWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
    }
}

