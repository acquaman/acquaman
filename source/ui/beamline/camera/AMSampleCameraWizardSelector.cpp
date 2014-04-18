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
	rotationWizardComplete_ = false;

	QVBoxLayout* sampleCameraWizardSelectorLayout = new QVBoxLayout();
	QFrame* beamWizardFrame = new QFrame();
	QHBoxLayout* beamWizardLayout = new QHBoxLayout();
	QFrame* cameraWizardFrame = new QFrame();
	QHBoxLayout* cameraWizardLayout = new QHBoxLayout();
	QFrame* samplePlateWizardFrame = new QFrame();
	QHBoxLayout* samplePlateWizardLayout = new QHBoxLayout();
	QFrame* rotationWizardFrame = new QFrame();
	QHBoxLayout* rotationWizardLayout = new QHBoxLayout();

	QFrame *reloadAllFrame = new QFrame();
	QHBoxLayout *reloadAllLayout = new QHBoxLayout();

	sampleCameraWizardSelectorLayout->setContentsMargins(0,0,0,0);
	beamWizardLayout->setContentsMargins(0,0,0,0);
	cameraWizardLayout->setContentsMargins(0,0,0,0);
	samplePlateWizardLayout->setContentsMargins(0,0,0,0);
	rotationWizardLayout->setContentsMargins(0,0,0,0);
	reloadAllLayout->setContentsMargins(0,0,0,0);

	cameraWizardLayout->addWidget(cameraWizardButton_ = new QPushButton("Setup Camera"));
	cameraWizardLayout->addStretch();
	cameraWizardLayout->addWidget(loadCameraConfigurationButton_ = new QPushButton("Load"));
	cameraWizardFrame->setLayout(cameraWizardLayout);

	rotationWizardLayout->addWidget(rotationWizardButton_ = new QPushButton("Setup Rotation"));
	rotationWizardLayout->addStretch();
	rotationWizardLayout->addWidget(loadRotationConfigurationButton_ = new QPushButton("Load"));
	rotationWizardFrame->setLayout(rotationWizardLayout);
	rotationWizardButton_->setEnabled(false);
	loadRotationConfigurationButton_->setEnabled(false);

	samplePlateWizardLayout->addWidget(samplePlateWizardButton_ = new QPushButton("Setup Sample Plate"));
	samplePlateWizardLayout->addStretch();
	samplePlateWizardLayout->addWidget(loadSamplePlateButton_ = new QPushButton("Load"));
	samplePlateWizardFrame->setLayout(samplePlateWizardLayout);
	samplePlateWizardButton_->setEnabled(false);
	loadSamplePlateButton_->setEnabled(false);

	beamWizardLayout->addWidget(beamWizardButton_ = new QPushButton("Setup Beam"));
	beamWizardLayout->addStretch();
	beamWizardLayout->addWidget(loadBeamConfigurationButton_ = new QPushButton("Load"));
	beamWizardFrame->setLayout(beamWizardLayout);
	beamWizardButton_->setEnabled(false);
	loadBeamConfigurationButton_->setEnabled(false);

	reloadAllLayout->addStretch();
	loadAllConfigurationsButton_ = new QPushButton("Reload All");
	reloadAllLayout->addWidget(loadAllConfigurationsButton_);
	reloadAllFrame->setLayout(reloadAllLayout);

	sampleCameraWizardSelectorLayout->addWidget(cameraWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(rotationWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(samplePlateWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(beamWizardFrame);
	sampleCameraWizardSelectorLayout->addWidget(reloadAllFrame);
	sampleCameraWizardSelectorLayout->addStretch();
	setLayout(sampleCameraWizardSelectorLayout);

	beamWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	cameraWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	samplePlateWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	rotationWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));

	connect(beamWizardButton_, SIGNAL(clicked()), this, SIGNAL(beamWizardPressed()));
	connect(cameraWizardButton_, SIGNAL(clicked()), this, SIGNAL(cameraWizardPressed()));
	connect(samplePlateWizardButton_, SIGNAL(clicked()), this, SIGNAL(samplePlateWizardPressed()));
	connect(rotationWizardButton_, SIGNAL(clicked()), this, SIGNAL(rotationWizardPressed()));

	connect(loadBeamConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadBeamConfiguration()));
	connect(loadCameraConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadCameraConfiguration()));
	connect(loadSamplePlateButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadSamplePlate()));
	connect(loadRotationConfigurationButton_, SIGNAL(clicked()), this, SIGNAL(requestLoadRotationConfiguration()));

	connect(loadAllConfigurationsButton_, SIGNAL(clicked()), this, SLOT(onLoadAllConfigurationsButtonClicked()));

	connect(this, SIGNAL(beamCompleteChanged()), this, SLOT(onBeamCompleteChanged()));
	connect(this, SIGNAL(cameraCompleteChanged()), this, SLOT(onCameraCompleteChanged()));
	connect(this, SIGNAL(samplePlateCompleteChanged()), this, SLOT(onSamplePlateCompleteChanged()));
	connect(this, SIGNAL(rotationCompleteChanged()), this, SLOT(onRotationCompleteChanged()));


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

bool AMSampleCameraWizardSelector::rotationWizardComplete() const
{
	return rotationWizardComplete_;
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

void AMSampleCameraWizardSelector::setRotationWizardComplete(const bool &complete)
{
	if(complete != rotationWizardComplete_)
	{
		rotationWizardComplete_ = complete;
		emit rotationCompleteChanged();
	}
}

void AMSampleCameraWizardSelector::onBeamWizardFinished()
{
	setBeamWizardComplete(true);
}

void AMSampleCameraWizardSelector::onCameraWizardFinished()
{
	setCameraWizardComplete(true);
}

void AMSampleCameraWizardSelector::onSamplePlateWizardFinished()
{
	setSamplePlateWizardComplete(true);
}

void AMSampleCameraWizardSelector::onRotationWizardFinished()
{
	setRotationWizardComplete(true);
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
	if(cameraWizardComplete())
	{
		cameraWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));

		rotationWizardButton_->setEnabled(true);
		loadRotationConfigurationButton_->setEnabled(true);
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

		beamWizardButton_->setEnabled(true);
		loadBeamConfigurationButton_->setEnabled(true);
	}
	else
	{
		samplePlateWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	}
}

void AMSampleCameraWizardSelector::onRotationCompleteChanged()
{
	if(rotationWizardComplete())
	{
		rotationWizardButton_->setIcon(QIcon(":/22x22/greenLEDOn.png"));

		samplePlateWizardButton_->setEnabled(true);
		loadSamplePlateButton_->setEnabled(true);
	}
	else
	{
		rotationWizardButton_->setIcon(QIcon(":/22x22/redLEDOn.png"));
	}
}

void AMSampleCameraWizardSelector::onLoadAllConfigurationsButtonClicked(){
	emit requestLoadCameraConfiguration();
	emit requestLoadRotationConfiguration();
	emit requestLoadSamplePlate();
	emit requestLoadBeamConfiguration();
}
