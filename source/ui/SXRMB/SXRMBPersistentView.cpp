#include "SXRMBPersistentView.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QGroupBox>

#include "beamline/SXRMB/SXRMBBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/AMMotorGroupView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

SXRMBPersistentView::SXRMBPersistentView(QWidget *parent) :
	QWidget(parent)
{
	beamOnAction_ = 0; //NULL
	beamOffAction_ = 0; //NULL

	// create persistent view component container
	mainVL_ = new QVBoxLayout();
	mainVL_->setContentsMargins(4, 2, 4, 2);

	mainGroupBox_ = new QGroupBox("SXRMB Beamline");
	mainGroupBox_->setLayout(mainVL_);

	mainGroupBoxVL_ = new QVBoxLayout();
	mainGroupBoxVL_->addWidget(mainGroupBox_);

	setLayout(mainGroupBoxVL_);
	setFixedWidth(350);

	//create Beamline Status components group
	statusControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->beamlineStatus(), 0, true);
	mainVL_->addWidget(statusControlEditor_);

	//create and add beam on/off buttons
	beamOnButton_ = new QPushButton("Beam On");
	beamOnButton_->setDisabled(true);
	beamOffButton_ = new QPushButton("Beam Off");
	beamOffButton_->setDisabled(true);

	QHBoxLayout *beamOnOffHL = new QHBoxLayout();
	beamOnOffHL->addWidget(beamOnButton_);
	beamOnOffHL->addWidget(beamOffButton_);

	mainVL_->addLayout(beamOnOffHL);

	// create energy component
	energyControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->energy());
	energyControlEditor_->setControlFormat('f', 2);

	mainVL_->addWidget(energyControlEditor_);

	// create motor groups
	motorGroupView_ = new AMMotorGroupView(SXRMBBeamline::sxrmb()->motorGroup());
	motorGroupView_->setMotorGroupView("Microprobe Stage - X, Z, Y");

	QVBoxLayout *motorGroupLayout = new QVBoxLayout();
	motorGroupLayout->setContentsMargins(4, 0, 4, 0);
	motorGroupLayout->addWidget(motorGroupView_);

	QGroupBox *motorGroupBox = new QGroupBox("MotorGroup");
	motorGroupBox->setLayout(motorGroupLayout);

	mainVL_->addWidget(motorGroupBox);

	//create scale groups
	i0DetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(17));
	i0DetectorSR570View_->setEnableCheckBoxVisibility(false);
	i0DetectorSR570View_->setAmplifierViewPrecision(3);
	i0DetectorSR570View_->setAmplifierViewFormat('g');

	teyDetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(18));
	teyDetectorSR570View_->setEnableCheckBoxVisibility(false);
	teyDetectorSR570View_->setAmplifierViewPrecision(3);
	teyDetectorSR570View_->setAmplifierViewFormat('g');

	QVBoxLayout *scalerGroupLayout = new QVBoxLayout();
	scalerGroupLayout->setContentsMargins(4, 0, 4, 0);
	scalerGroupLayout->addWidget(i0DetectorSR570View_);
	scalerGroupLayout->addWidget(teyDetectorSR570View_);

	QGroupBox *scalerGroupBox = new QGroupBox("Scalers");
	scalerGroupBox->setLayout(scalerGroupLayout);

	mainVL_->addWidget(scalerGroupBox);

	// add stretch for display purpose
	mainVL_->addStretch();

	// connect to signals
	connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
	connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffButtonClicked()));
}

SXRMBPersistentView::~SXRMBPersistentView()
{
}

void SXRMBPersistentView::onBeamOnButtonClicked(){
	// DON'T DO ANYTHING RIGHT NOW
	return;

	if(beamOnAction_)
		return;
	beamOnAction_ = SXRMBBeamline::sxrmb()->createBeamOnActions();
	connect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
	connect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));
	beamOnAction_->start();
}

void SXRMBPersistentView::onBeamOnActionFinished(){
	disconnect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
	disconnect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));
	beamOnAction_->deleteLater();
	beamOnAction_ = 0; //NULL
}

void SXRMBPersistentView::onBeamOffButtonClicked(){
	// DON'T DO ANYTHING RIGHT NOW
	return;

	if(beamOffAction_)
		return;
	beamOffAction_ = SXRMBBeamline::sxrmb()->createBeamOffActions();
	connect(beamOffAction_, SIGNAL(succeeded()), this, SLOT(onBeamOffActionFinished()));
	connect(beamOffAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));
	beamOffAction_->start();
}

void SXRMBPersistentView::onBeamOffActionFinished(){
	disconnect(beamOffAction_, SIGNAL(succeeded()), this, SLOT(onBeamOffActionFinished()));
	disconnect(beamOffAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));
	beamOffAction_->deleteLater();
	beamOffAction_ = 0; //NULL
}
