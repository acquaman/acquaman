#include "SXRMBPersistentView.h"

#include <QBoxLayout>
#include <QGroupBox>

#include "beamline/AMControl.h"
#include "beamline/SXRMB/SXRMBBeamline.h"

#include "ui/AMMotorGroupView.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"
#include "ui/CLS/CLSHVControlGroupView.h"

#include "util/AMErrorMonitor.h"

SXRMBPersistentView::SXRMBPersistentView(QWidget *parent) :
	QWidget(parent)
{
	beamOnAction_ = 0; //NULL
	beamOffAction_ = 0; //NULL

	// create persistent view component container
	contentLayout_ = new QVBoxLayout();
	contentLayout_->setContentsMargins(4, 2, 4, 2);

	layoutBeamlineStatus();
	layoutBeamlineEnergy();
	layoutMotorGroup();
	layoutScalers();
	layoutHVControls();

	// add stretch for display purpose
	contentLayout_->addStretch();

	// create the persistent view group box
	QGroupBox *persistentViewGroupBox_ = new QGroupBox("SXRMB Beamline");
	persistentViewGroupBox_->setLayout(contentLayout_);

	/// Main layout holding overall group box
	QVBoxLayout *widgetLayout = new QVBoxLayout();
	widgetLayout->addWidget(persistentViewGroupBox_);

	setLayout(widgetLayout);
	setFixedWidth(400);

	// connect to signals and initialization
	connect(SXRMBBeamline::sxrmb(), SIGNAL(endstationChanged(SXRMB::Endstation, SXRMB::Endstation)), this, SLOT(onBeamlineEndstationChanged(SXRMB::Endstation, SXRMB::Endstation)));

	onBeamlineEndstationChanged(SXRMBBeamline::sxrmb()->currentEndstation(), SXRMBBeamline::sxrmb()->currentEndstation());
}

SXRMBPersistentView::~SXRMBPersistentView()
{
}

void SXRMBPersistentView::onBeamlineEndstationChanged(SXRMB::Endstation fromEndstation, SXRMB::Endstation toEndstation)
{
	Q_UNUSED(fromEndstation)

	QString endstationName = SXRMB::sxrmbEndstationName(SXRMBBeamline::sxrmb()->currentEndstation());
	QString motorGroupName = SXRMBBeamline::sxrmb()->currentMotorGroupName();

	endstationLabel_->setText("Endstation: <b><i>" + endstationName + "</i></b>");

	motorGroupView_->setSelectedGroupObject(motorGroupName);

	beamlineI0DetectorSR570View_->setVisible(toEndstation != SXRMB::Microprobe);
	i0DetectorSR570View_->setVisible(toEndstation != SXRMB::SolidState);
	teyDetectorSR570View_->setVisible((toEndstation != SXRMB::AmbiantWithGasChamber && toEndstation != SXRMB::AmbiantWithoutGasChamber));
	transmissionDetectorSR570View_->setVisible((toEndstation == SXRMB::AmbiantWithGasChamber || toEndstation == SXRMB::AmbiantWithoutGasChamber));
}

void SXRMBPersistentView::layoutBeamlineStatus()
{
	SXRMBBeamline *sxrmbBl = SXRMBBeamline::sxrmb();

	// create beamline endstation label
	endstationLabel_ = new QLabel("Endstation: ");
	endstationLabel_->setMargin(5);

	// create the beamline status view with beam on/off actions
	CLSBeamlineStatusView *beamlineStatusView = new CLSBeamlineStatusView(sxrmbBl, true, true);
	beamlineStatusView->enableBeamOnOffActions();
	connect(beamlineStatusView, SIGNAL(controlClicked(AMControl*)), this, SIGNAL(beamlineStatusControlClicked(AMControl*)) );

	// layout the components
	contentLayout_->addWidget(endstationLabel_);
	contentLayout_->addWidget(beamlineStatusView);
}

void SXRMBPersistentView::layoutBeamlineEnergy()
{
	// create energy component
	AMExtendedControlEditor *energyControlEditor = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->energy());
	energyControlEditor->setControlFormat('f', 2);
	energyControlEditor->setUnits("eV");

	contentLayout_->addWidget(energyControlEditor);
}

void SXRMBPersistentView::layoutMotorGroup()
{
	// create motor groups
	motorGroupView_ = new AMMotorGroupView(SXRMBBeamline::sxrmb()->motorGroup(), AMMotorGroupView::CompactView);
	motorGroupView_->setSelectedGroupObject(SXRMBBeamline::sxrmb()->currentMotorGroupName());
	motorGroupView_->hideMotorGroupSelection();

	QVBoxLayout *motorGroupLayout = new QVBoxLayout();
	motorGroupLayout->setContentsMargins(4, 0, 4, 0);
	motorGroupLayout->addWidget(motorGroupView_);

	QGroupBox *motorGroupBox = new QGroupBox("Motor Group");
	motorGroupBox->setLayout(motorGroupLayout);

	contentLayout_->addWidget(motorGroupBox);
}

void SXRMBPersistentView::layoutScalers()
{
	//create scale groups
	beamlineI0DetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(16));
	beamlineI0DetectorSR570View_->setEnableCheckBoxVisibility(false);
	beamlineI0DetectorSR570View_->setAmplifierViewPrecision(3);
	beamlineI0DetectorSR570View_->setAmplifierViewFormat('g');

	i0DetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(17));
	i0DetectorSR570View_->setEnableCheckBoxVisibility(false);
	i0DetectorSR570View_->setAmplifierViewPrecision(3);
	i0DetectorSR570View_->setAmplifierViewFormat('g');

	teyDetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(18));
	teyDetectorSR570View_->setEnableCheckBoxVisibility(false);
	teyDetectorSR570View_->setAmplifierViewPrecision(3);
	teyDetectorSR570View_->setAmplifierViewFormat('g');

	transmissionDetectorSR570View_ = new CLSSIS3820ScalerChannelView(SXRMBBeamline::sxrmb()->scaler()->channelAt(19));
	transmissionDetectorSR570View_->setEnableCheckBoxVisibility(false);
	transmissionDetectorSR570View_->setAmplifierViewPrecision(3);
	transmissionDetectorSR570View_->setAmplifierViewFormat('g');

	QVBoxLayout *scalerGroupLayout = new QVBoxLayout();
	scalerGroupLayout->setContentsMargins(4, 0, 4, 0);
	scalerGroupLayout->addWidget(beamlineI0DetectorSR570View_);
	scalerGroupLayout->addWidget(i0DetectorSR570View_);
	scalerGroupLayout->addWidget(teyDetectorSR570View_);
	scalerGroupLayout->addWidget(transmissionDetectorSR570View_);

	QGroupBox *scalerGroupBox = new QGroupBox("Scalers");
	scalerGroupBox->setLayout(scalerGroupLayout);

	contentLayout_->addWidget(scalerGroupBox);
}

void SXRMBPersistentView::layoutHVControls()
{
	CLSHVControlGroupView *hvControlView = new CLSHVControlGroupView(SXRMBBeamline::sxrmb()->beamlinePersistentHVControlSet(), true);

	contentLayout_->addWidget(hvControlView);

}
