#include "SXRMBPersistentView.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QGroupBox>

#include "beamline/SXRMB/SXRMBBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/AMMotorGroupView.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

#include "util/AMErrorMonitor.h"

SXRMBPersistentView::SXRMBPersistentView(QWidget *parent) :
	QWidget(parent)
{
	beamOnAction_ = 0; //NULL
	beamOffAction_ = 0; //NULL

	// create persistent view component container
	mainVL_ = new QVBoxLayout();
	mainVL_->setContentsMargins(4, 2, 4, 2);

	layoutBeamlineStatus();
	layoutBeamlineEnergy();
	layoutMotorGroup();
	layoutScalers();

	// add stretch for display purpose
	mainVL_->addStretch();

	mainGroupBox_ = new QGroupBox("SXRMB Beamline");
	mainGroupBox_->setLayout(mainVL_);

	mainGroupBoxVL_ = new QVBoxLayout();
	mainGroupBoxVL_->addWidget(mainGroupBox_);

	setLayout(mainGroupBoxVL_);
	setFixedWidth(350);

	// connect to signals
	connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnButtonClicked()));
	connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffButtonClicked()));
	connect(SXRMBBeamline::sxrmb(), SIGNAL(endStationChanged(SXRMB::Endstation)), this, SLOT(onBeamlineEndStationChanged(SXRMB::Endstation)));

	onBeamlineEndStationChanged(SXRMBBeamline::sxrmb()->currentEndStation());
}

SXRMBPersistentView::~SXRMBPersistentView()
{
}

void SXRMBPersistentView::onBeamOnButtonClicked(){
	if(beamOnAction_)
		return;

	beamOnAction_ = SXRMBBeamline::sxrmb()->createBeamOnActions();
	if (beamOnAction_) {
		connect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
		connect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));
		beamOnAction_->start();
	} else {
		AMErrorMon::information(this, 0, QString("Failed to create the beam on actions due to either unconnected or openned valves."));
	}
}

void SXRMBPersistentView::onBeamOnActionFinished(){
	disconnect(beamOnAction_, SIGNAL(succeeded()), this, SLOT(onBeamOnActionFinished()));
	disconnect(beamOnAction_, SIGNAL(failed()), this, SLOT(onBeamOnActionFinished()));

	beamOnAction_->deleteLater();
	beamOnAction_ = 0; //NULL
}

void SXRMBPersistentView::onBeamOffButtonClicked(){
	if(beamOffAction_)
		return;

	beamOffAction_ = SXRMBBeamline::sxrmb()->createBeamOffActions();
	if (beamOffAction_) {
		connect(beamOffAction_, SIGNAL(succeeded()), this, SLOT(onBeamOffActionFinished()));
		connect(beamOffAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));
		beamOffAction_->start();
	} else {
		AMErrorMon::information(this, 0, QString("Failed to create the beam off actions due to either unconnected valves or closed shutters."));
	}
}

void SXRMBPersistentView::onBeamOffActionFinished(){
	disconnect(beamOffAction_, SIGNAL(succeeded()), this, SLOT(onBeamOffActionFinished()));
	disconnect(beamOffAction_, SIGNAL(failed()), this, SLOT(onBeamOffActionFinished()));

	beamOffAction_->deleteLater();
	beamOffAction_ = 0; //NULL
}

void SXRMBPersistentView::onBeamlineEndStationChanged(SXRMB::Endstation endstation)
{
	QString endStationName = SXRMB::sxrmbEndStationName(SXRMBBeamline::sxrmb()->currentEndStation());
	QString motorGroupName = SXRMBBeamline::sxrmb()->currentMotorGroupName();

	endStationLabel_->setText("Endstation: <b><i>" + endStationName + "</i></b>");
	motorGroupView_->setMotorGroupView(motorGroupName);

	beamlineI0DetectorSR570View_->setVisible(endstation != SXRMB::Microprobe);
	i0DetectorSR570View_->setVisible(endstation != SXRMB::SolidState);
	teyDetectorSR570View_->setVisible((endstation != SXRMB::AmbiantWithGasChamber && endstation != SXRMB::AmbiantWithoutGasChamber));
	transmissionDetectorSR570View_->setVisible((endstation == SXRMB::AmbiantWithGasChamber || endstation == SXRMB::AmbiantWithoutGasChamber));
}

void SXRMBPersistentView::layoutBeamlineStatus()
{
	// create beamline endstation label
	endStationLabel_ = new QLabel("Endstation: ");
	endStationLabel_->setMargin(5);
	mainVL_->addWidget(endStationLabel_);

	//create Beamline Status components group
	statusControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->beamlineStatus(), 0, true);
	mainVL_->addWidget(statusControlEditor_);

	//create and add beam on/off buttons
	beamOnButton_ = new QPushButton("Beam On");
	beamOffButton_ = new QPushButton("Beam Off");

	QHBoxLayout *beamOnOffHL = new QHBoxLayout();
	beamOnOffHL->addWidget(beamOnButton_);
	beamOnOffHL->addWidget(beamOffButton_);

	mainVL_->addLayout(beamOnOffHL);
}

void SXRMBPersistentView::layoutBeamlineEnergy()
{
	// create energy component
	energyControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->energy());
	energyControlEditor_->setControlFormat('f', 2);

	mainVL_->addWidget(energyControlEditor_);
}

void SXRMBPersistentView::layoutMotorGroup()
{
	// create motor groups
	motorGroupView_ = new AMMotorGroupView(SXRMBBeamline::sxrmb()->motorGroup(), AMMotorGroupView::Exclusive);
	motorGroupView_->showAvailableMotorGroupChoices(false);
	motorGroupView_->setMotorGroupView(SXRMBBeamline::sxrmb()->currentMotorGroupName());

	QVBoxLayout *motorGroupLayout = new QVBoxLayout();
	motorGroupLayout->setContentsMargins(4, 0, 4, 0);
	motorGroupLayout->addWidget(motorGroupView_);

	QGroupBox *motorGroupBox = new QGroupBox("MotorGroup");
	motorGroupBox->setLayout(motorGroupLayout);

	mainVL_->addWidget(motorGroupBox);
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

	mainVL_->addWidget(scalerGroupBox);
}
