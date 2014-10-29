#include "SXRMBPersistentView.h"

#include <QBoxLayout>
#include <QGroupBox>

#include "beamline/SXRMB/SXRMBBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/AMMotorGroupView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

SXRMBPersistentView::SXRMBPersistentView(QWidget *parent) :
	QWidget(parent)
{
	mainGroupBox_ = new QGroupBox("SXRMB Beamline");
	mainGroupBoxVL_ = new QVBoxLayout();
	mainGroupBoxVL_->addWidget(mainGroupBox_);
	setLayout(mainGroupBoxVL_);

	energyControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->energy());
	energyControlEditor_->setControlFormat('f', 2);
	statusControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->beamlineStatus(), 0, true);

	motorGroupView_ = new AMMotorGroupView(SXRMBBeamline::sxrmb()->motorGroup());
	motorGroupView_->setMotorGroupView("Microprobe Stage - X, Z, Y");

	i0DetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(17));
	teyDetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(18));
	i0DetectorSR570View_->setEnableCheckBoxVisibility(false);
	teyDetectorSR570View_->setEnableCheckBoxVisibility(false);

	mainVL_ = new QVBoxLayout();
	mainVL_->setContentsMargins(2, 2, 2, 2);

	mainVL_->addWidget(statusControlEditor_);
	mainVL_->addWidget(energyControlEditor_);
	mainVL_->addWidget(motorGroupView_);
	mainVL_->addWidget(i0DetectorSR570View_);
	mainVL_->addWidget(teyDetectorSR570View_);
	mainVL_->addStretch();

	setFixedWidth(350);
	mainGroupBox_->setLayout(mainVL_);
}

SXRMBPersistentView::~SXRMBPersistentView()
{
}
