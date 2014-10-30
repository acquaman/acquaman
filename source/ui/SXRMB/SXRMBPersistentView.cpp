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

	mainGroupBox_ = new QGroupBox("SXRMB Beamline");
	mainGroupBoxVL_ = new QVBoxLayout();
	mainGroupBoxVL_->addWidget(mainGroupBox_);
	setLayout(mainGroupBoxVL_);

	statusControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->beamlineStatus(), 0, true);

	beamOnButton_ = new QPushButton("Beam On");
	beamOffButton_ = new QPushButton("Beam Off");
	QHBoxLayout *beamOnOffHL = new QHBoxLayout();
	beamOnOffHL->addWidget(beamOnButton_);
	beamOnOffHL->addWidget(beamOffButton_);

	beamOnButton_->setDisabled(true);
	beamOffButton_->setDisabled(true);

	energyControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->energy());
	energyControlEditor_->setControlFormat('f', 2);

	motorGroupView_ = new AMMotorGroupView(SXRMBBeamline::sxrmb()->motorGroup());
	motorGroupView_->setMotorGroupView("Microprobe Stage - X, Z, Y");

	i0DetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(17));
	teyDetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(18));
	i0DetectorSR570View_->setEnableCheckBoxVisibility(false);
	teyDetectorSR570View_->setEnableCheckBoxVisibility(false);
	i0DetectorSR570View_->setAmplifierViewPrecision(3);
	teyDetectorSR570View_->setAmplifierViewPrecision(3);
	i0DetectorSR570View_->setAmplifierViewFormat('g');
	teyDetectorSR570View_->setAmplifierViewFormat('g');

	mainVL_ = new QVBoxLayout();
	mainVL_->setContentsMargins(2, 2, 2, 2);

	mainVL_->addWidget(statusControlEditor_);
	mainVL_->addLayout(beamOnOffHL);
	mainVL_->addWidget(energyControlEditor_);
	mainVL_->addWidget(motorGroupView_);
	mainVL_->addWidget(i0DetectorSR570View_);
	mainVL_->addWidget(teyDetectorSR570View_);
	mainVL_->addStretch();

	setFixedWidth(350);
	mainGroupBox_->setLayout(mainVL_);

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
