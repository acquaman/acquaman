#include "SGMSampleManipulatorView.h"
#include "beamline/SGM/SGMBeamline.h"

#include <QLabel>
#include <QPalette>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>

#include "beamline/AMBeamlineListAction.h"
#include "beamline/CLS/CLSVMEMotor.h"

SGMSampleManipulatorView::SGMSampleManipulatorView(bool bigButtons, QWidget *parent) :
		AMSampleManipulatorView("SGM SSA Endstation", SGMBeamline::sgm()->ssaManipulatorSet(), parent )
{
	lastHVValue_ = 0;
	isJogging_ = false;
	jogStep_ = 0.1;

	upIcon_ = QIcon(":/go-up.png");
	mUpButton_ = new QPushButton(upIcon_, "");
	mUpButton_->setToolTip("Move Up");
	downIcon_ = QIcon(":/go-down.png");
	mDownButton_ = new QPushButton(downIcon_, "");
	mDownButton_->setToolTip("Move Down");
	inboardIcon_ = QIcon(":/go-previous.png");
	mInboardButton_ = new QPushButton(inboardIcon_, "");
	mInboardButton_->setToolTip("Move Inboard");
	outboardIcon_ = QIcon(":/go-next.png");
	mOutboardButton_ = new QPushButton(outboardIcon_, "");
	mOutboardButton_->setToolTip("Move Outboard");
	upstreamIcon_ = QIcon(":/ArrowPlaneOut.png");
	mUpstreamButton_ = new QPushButton(upstreamIcon_, "");
	mUpstreamButton_->setToolTip("Move Upstream");
	downstreamIcon_ = QIcon(":/ArrowPlaneIn.png");
	mDownstreamButton_ = new QPushButton(downstreamIcon_, "");
	mDownstreamButton_->setToolTip("Move Downstream");
	cwIcon_ = QIcon(":/ArrowCW.png");
	mCWButton_ = new QPushButton(cwIcon_, "");
	mCWButton_->setToolTip("Move Clockwise");
	ccwIcon_ = QIcon(":/ArrowCCW.png");
	mCCWButton_ = new QPushButton(ccwIcon_, "");
	mCCWButton_->setToolTip("Move Counter-Clockwise");

	stopAllButton_ = new QPushButton("STOP");
	QPalette sabp = stopAllButton_->palette();
	sabp.setColor(QPalette::ButtonText, Qt::red);
	stopAllButton_->setPalette(sabp);

	jogBox_ = new QCheckBox("Jog");
	jogSettingComboBox_ = new QComboBox();
	jogSettingComboBox_->addItem("0.1 mm");
	jogSettingComboBox_->addItem("0.25 mm");
	jogSettingComboBox_->addItem("0.5 mm");
	jogSettingComboBox_->addItem("1.0 mm");

	transferPositionButton_ = new QPushButton("Transfer Position");
	transferPositionActions_ = 0;//NULL
	measurePositionButton_ = new QPushButton("Measure Position");
	measurementPositionActions_ = 0;//NULL

	hvButton_ = new QPushButton("HV Toggle");

	/*
	mVerticalCtrl_ = manipulator_->controlNamed("ssaManipulatorZ");
	mHorizontalCtrl_ = manipulator_->controlNamed("ssaManipulatorX");
	mInPlaneCtrl_ = manipulator_->controlNamed("ssaManipulatorY");
	mRotationCtrl_ = manipulator_->controlNamed("ssaManipulatorRot");
	*/
	mVerticalCtrl_ = qobject_cast<CLSVMEMotor*>(manipulator_->controlNamed("ssaManipulatorZ"));
	mHorizontalCtrl_ = qobject_cast<CLSVMEMotor*>(manipulator_->controlNamed("ssaManipulatorX"));
	mInPlaneCtrl_ = qobject_cast<CLSVMEMotor*>(manipulator_->controlNamed("ssaManipulatorY"));
	mRotationCtrl_ = qobject_cast<CLSVMEMotor*>(manipulator_->controlNamed("ssaManipulatorRot"));

	mVerticalNC_ = new AMControlEditor(mVerticalCtrl_);
	mHorizontalNC_ = new AMControlEditor(mHorizontalCtrl_);
	mInPlaneNC_ = new AMControlEditor(mInPlaneCtrl_);
	mRotationNC_ = new AMControlEditor(mRotationCtrl_);

	illuminatorSlider_ = new QSlider(Qt::Horizontal);
	illuminatorSlider_->setRange(0, 100);
	illuminatorPresets_ = new QButtonGroup();
	illuminatorOff_ = new QToolButton();
	illuminatorOff_->setText("Off");
	illuminatorOff_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorDim_ = new QToolButton();
	illuminatorDim_->setText("Dim");
	illuminatorDim_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorMid_ = new QToolButton();
	illuminatorMid_->setText("Mid");
	illuminatorMid_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorOn_ = new QToolButton();
	illuminatorOn_->setText("On");
	illuminatorOn_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
	illuminatorPresets_->addButton(illuminatorOff_, 0);
	illuminatorPresets_->addButton(illuminatorDim_, 1);
	illuminatorPresets_->addButton(illuminatorMid_, 2);
	illuminatorPresets_->addButton(illuminatorOn_, 3);
	QHBoxLayout *ipl = new QHBoxLayout();
	ipl->addWidget(illuminatorOff_);
	ipl->addWidget(illuminatorDim_);
	ipl->addWidget(illuminatorMid_);
	ipl->addWidget(illuminatorOn_);

	if(bigButtons){
		mUpButton_->setMinimumSize(125, 125);
		mDownButton_->setMinimumSize(125, 125);
		mInboardButton_->setMinimumSize(125, 125);
		mOutboardButton_->setMinimumSize(125, 125);
		mUpstreamButton_->setMinimumSize(125, 125);
		mDownstreamButton_->setMinimumSize(125, 125);
		mCWButton_->setMinimumSize(125, 125);
		mCCWButton_->setMinimumSize(125, 125);
		stopAllButton_->setMinimumSize(125, 125);

		transferPositionButton_->setMinimumHeight(100);
		measurePositionButton_->setMinimumHeight(100);
		hvButton_->setMinimumHeight(100);

		illuminatorOff_->setMinimumHeight(75);
		illuminatorDim_->setMinimumHeight(75);
		illuminatorMid_->setMinimumHeight(75);
		illuminatorOn_->setMinimumHeight(75);
	}

	connect(mUpButton_, SIGNAL(pressed()), this, SLOT(onMUpButtonPressed()));
	connect(mUpButton_, SIGNAL(released()), this, SLOT(onMUpButtonReleased()));
	connect(mDownButton_, SIGNAL(pressed()), this, SLOT(onMDownButtonPressed()));
	connect(mDownButton_, SIGNAL(released()), this, SLOT(onMDownButtonReleased()));
	connect(mInboardButton_, SIGNAL(pressed()), this, SLOT(onMInboardButtonPressed()));
	connect(mInboardButton_, SIGNAL(released()), this, SLOT(onMInboardButtonReleased()));
	connect(mOutboardButton_, SIGNAL(pressed()), this, SLOT(onMOutboardButtonPressed()));
	connect(mOutboardButton_, SIGNAL(released()), this, SLOT(onMOutboardButtonReleased()));
	connect(mUpstreamButton_, SIGNAL(pressed()), this, SLOT(onMUpstreamButtonPressed()));
	connect(mUpstreamButton_, SIGNAL(released()), this, SLOT(onMUpstreamButtonReleased()));
	connect(mDownstreamButton_, SIGNAL(pressed()), this, SLOT(onMDownstreamButtonPressed()));
	connect(mDownstreamButton_, SIGNAL(released()), this, SLOT(onMDownstreamButtonReleased()));
	connect(mCWButton_, SIGNAL(pressed()), this, SLOT(onMCWButtonPressed()));
	connect(mCWButton_, SIGNAL(released()), this, SLOT(onMCWButtonReleased()));
	connect(mCCWButton_, SIGNAL(pressed()), this, SLOT(onMCCWButtonPressed()));
	connect(mCCWButton_, SIGNAL(released()), this, SLOT(onMCCWButtonReleased()));

	connect(stopAllButton_, SIGNAL(clicked()), this, SLOT(onStopAllButtonClicked()));
	connect(jogBox_, SIGNAL(toggled(bool)), this, SLOT(onJogButtonChecked(bool)));
	connect(jogSettingComboBox_, SIGNAL(activated(int)), this, SLOT(onJogSettingComboBoxChanged(int)));
	connect(transferPositionButton_, SIGNAL(clicked()), this, SLOT(onTransferPositionButtonClicked()));
	connect(measurePositionButton_, SIGNAL(clicked()), this, SLOT(onMeasurePositionButtonClicked()));
	connect(hvButton_, SIGNAL(clicked()), this, SLOT(onHVButtonClicked()));
	connect(SGMBeamline::sgm(), SIGNAL(detectorHVChanged()), this, SLOT(onHVStateChanged()));

	connect(illuminatorSlider_, SIGNAL(sliderMoved(int)), this, SLOT(onIlluminatorSliderValueMoved(int)));
	connect(SGMBeamline::sgm()->ssaIllumination(), SIGNAL(valueChanged(double)), this, SLOT(onIlluminatorFeedbackChanged(double)));
	connect(illuminatorPresets_, SIGNAL(buttonClicked(int)), this, SLOT(onIlluminatorPreset(int)));

	gl_ = new QGridLayout();
	gl_->addWidget(mUpButton_,		0, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mDownButton_,		4, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mInboardButton_,		2, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mOutboardButton_,	2, 4, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mUpstreamButton_,	4, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mDownstreamButton_,	0, 4, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mCWButton_,		0, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mCCWButton_,		4, 4, 1, 1, Qt::AlignCenter);
	gl_->addWidget(stopAllButton_,		2, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(jogBox_,			5, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(jogSettingComboBox_,	5, 2, 1, 1, Qt::AlignCenter);
	gl_->setRowMinimumHeight(1, 10);
	gl_->setRowMinimumHeight(3, 10);
	gl_->setColumnMinimumWidth(1, 5);
	gl_->setColumnMinimumWidth(3, 5);

	QVBoxLayout *vl1 = new QVBoxLayout();
	vl1->addWidget(mVerticalNC_);
	vl1->addWidget(mHorizontalNC_);
	vl1->addWidget(mInPlaneNC_);
	vl1->addWidget(mRotationNC_);

	QVBoxLayout *vl2 = new QVBoxLayout();
	vl2->addLayout(gl_);
	vl2->addStretch(10);
	if(!bigButtons){
		vl2->addWidget(illuminatorSlider_);
		QLabel *illuminatorLabel = new QLabel("Chamber Light");
		illuminatorLabel->setAlignment(Qt::AlignCenter);
		vl2->addWidget(illuminatorLabel);
		vl2->addLayout(ipl);
		vl2->addWidget(transferPositionButton_);
		vl2->addWidget(measurePositionButton_);
		vl2->addWidget(hvButton_);
	}
	else{
		vl1->addStretch(10);
		vl1->addWidget(illuminatorSlider_);
		QLabel *illuminatorLabel = new QLabel("Chamber Light");
		illuminatorLabel->setAlignment(Qt::AlignCenter);
		vl1->addWidget(illuminatorLabel);
		vl1->addLayout(ipl);
		vl1->addWidget(transferPositionButton_);
		vl1->addWidget(measurePositionButton_);
		vl1->addWidget(hvButton_);
	}

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addLayout(vl2);
	hl->addLayout(vl1);
	hl->setContentsMargins(0,0,0,0);
	hl->setSpacing(1);

	gl_->setContentsMargins(10, 10, 5, 10);
	gl_->setSpacing(5);
	vl1->setContentsMargins(5, 10, 10, 10);
	vl1->setSpacing(5);

	setLayout(hl);
}

void SGMSampleManipulatorView::onMUpButtonPressed(){
	if(isJogging_)
		mVerticalCtrl_->move(mVerticalCtrl_->value()+jogStep_);
	else{
		/*
		//mVerticalCtrl_->move(mVerticalCtrl_->maximumValue());
		SGMBeamline::sgm()->ssaManipulatorZVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorZVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorZAcceleration()->move(2000);
		mVerticalCtrl_->move(40);
		*/
		mVerticalCtrl_->setVelocity(3000);
		mVerticalCtrl_->setAcceleration(2000);
		mVerticalCtrl_->move(40);
	}
}

void SGMSampleManipulatorView::onMUpButtonReleased(){
	if(!isJogging_){
		mVerticalCtrl_->stop();
		mVerticalCtrl_->setVelocity(10000);
		mVerticalCtrl_->setAcceleration(5000);
		/*
		SGMBeamline::sgm()->ssaManipulatorZVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorZVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorZAcceleration()->move(5000);
		//mVerticalCtrl_->move(mVerticalCtrl_->value());
		*/
	}
}

void SGMSampleManipulatorView::onMDownButtonPressed(){
	if(isJogging_)
		mVerticalCtrl_->move(mVerticalCtrl_->value()-jogStep_);
	else{
		/*
		//mVerticalCtrl_->move(mVerticalCtrl_->minimumValue());
		SGMBeamline::sgm()->ssaManipulatorZVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorZVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorZAcceleration()->move(2000);
		mVerticalCtrl_->move(-80);
		*/
		mVerticalCtrl_->setVelocity(3000);
		mVerticalCtrl_->setAcceleration(2000);
		mVerticalCtrl_->move(-80);
	}
}

void SGMSampleManipulatorView::onMDownButtonReleased(){
	if(!isJogging_){
		mVerticalCtrl_->stop();
		/*
		SGMBeamline::sgm()->ssaManipulatorZVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorZVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorZAcceleration()->move(5000);
		//mVerticalCtrl_->move(mVerticalCtrl_->value());
		*/
		mVerticalCtrl_->setVelocity(10000);
		mVerticalCtrl_->setAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMInboardButtonPressed(){
	if(isJogging_)
		mHorizontalCtrl_->move(mHorizontalCtrl_->value()-jogStep_);
	else{
		/*
		//mHorizontalCtrl_->move(mHorizontalCtrl_->maximumValue());
		SGMBeamline::sgm()->ssaManipulatorXVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorXVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorXAcceleration()->move(2000);
		mHorizontalCtrl_->move(-10.0);
		*/
		mHorizontalCtrl_->setVelocity(3000);
		mHorizontalCtrl_->setAcceleration(2000);
		mHorizontalCtrl_->move(-10.0);
	}
}

void SGMSampleManipulatorView::onMInboardButtonReleased(){
	if(!isJogging_){
		mHorizontalCtrl_->stop();
		/*
		SGMBeamline::sgm()->ssaManipulatorXVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorXVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorXAcceleration()->move(5000);
		//mHorizontalCtrl_->move(mHorizontalCtrl_->value());
		*/
		mHorizontalCtrl_->setVelocity(10000);
		mHorizontalCtrl_->setAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMOutboardButtonPressed(){
	if(isJogging_)
		mHorizontalCtrl_->move(mHorizontalCtrl_->value()+jogStep_);
	else{
		/*
		//mHorizontalCtrl_->move(mHorizontalCtrl_->minimumValue());
		SGMBeamline::sgm()->ssaManipulatorXVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorXVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorXAcceleration()->move(2000);
		mHorizontalCtrl_->move(10.0);
		*/
		mHorizontalCtrl_->setVelocity(3000);
		mHorizontalCtrl_->setAcceleration(2000);
		mHorizontalCtrl_->move(10.0);
	}
}

void SGMSampleManipulatorView::onMOutboardButtonReleased(){
	if(!isJogging_){
		mHorizontalCtrl_->stop();
		/*
		SGMBeamline::sgm()->ssaManipulatorXVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorXVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorXAcceleration()->move(5000);
		//mHorizontalCtrl_->move(mHorizontalCtrl_->value());
		*/
		mHorizontalCtrl_->setVelocity(10000);
		mHorizontalCtrl_->setAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMUpstreamButtonPressed(){
	if(isJogging_)
		mInPlaneCtrl_->move(mInPlaneCtrl_->value()-jogStep_);
	else{
		/*
		//mInPlaneCtrl_->move(mInPlaneCtrl_->maximumValue());
		SGMBeamline::sgm()->ssaManipulatorYVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorYVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorYAcceleration()->move(2000);
		mInPlaneCtrl_->move(-15.0);
		*/
		mInPlaneCtrl_->setVelocity(3000);
		mInPlaneCtrl_->setAcceleration(2000);
		mInPlaneCtrl_->move(-15.0);
	}
}

void SGMSampleManipulatorView::onMUpstreamButtonReleased(){
	if(!isJogging_){
		mInPlaneCtrl_->stop();
		/*
		SGMBeamline::sgm()->ssaManipulatorYVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorYVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorYAcceleration()->move(5000);
		//mInPlaneCtrl_->move(mInPlaneCtrl_->value());
		*/
		mInPlaneCtrl_->setVelocity(10000);
		mInPlaneCtrl_->setAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMDownstreamButtonPressed(){
	if(isJogging_)
		mInPlaneCtrl_->move(mInPlaneCtrl_->value()+jogStep_);
	else{
		/*
		//mInPlaneCtrl_->move(mInPlaneCtrl_->minimumValue());
		SGMBeamline::sgm()->ssaManipulatorYVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorYVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorYAcceleration()->move(2000);
		mInPlaneCtrl_->move(10.0);
		*/
		mInPlaneCtrl_->setVelocity(3000);
		mInPlaneCtrl_->setAcceleration(2000);
		mInPlaneCtrl_->move(10.0);
	}
}

void SGMSampleManipulatorView::onMDownstreamButtonReleased(){
	if(!isJogging_){
		mInPlaneCtrl_->stop();
		/*
		SGMBeamline::sgm()->ssaManipulatorYVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorYVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorYAcceleration()->move(5000);
		//mInPlaneCtrl_->move(mInPlaneCtrl_->value());
		*/
		mInPlaneCtrl_->setVelocity(10000);
		mInPlaneCtrl_->setAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMCWButtonPressed(){
	if(isJogging_)
		mRotationCtrl_->move(mRotationCtrl_->value()-jogStep_);
	else{
		/*
		//mRotationCtrl_->move(mRotationCtrl_->minimumValue());
		SGMBeamline::sgm()->ssaManipulatorRotVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorRotVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorRotAcceleration()->move(2000);
		mRotationCtrl_->move(-360.0);
		*/
		mRotationCtrl_->setVelocity(3000);
		mRotationCtrl_->setAcceleration(2000);
		mRotationCtrl_->move(-360.0);
	}
}

void SGMSampleManipulatorView::onMCWButtonReleased(){
	if(!isJogging_){
		mRotationCtrl_->stop();
		/*
		SGMBeamline::sgm()->ssaManipulatorRotVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorRotVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorRotAcceleration()->move(5000);
		//mRotationCtrl_->move(mRotationCtrl_->value());
		*/
		mRotationCtrl_->setVelocity(10000);
		mRotationCtrl_->setAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onMCCWButtonPressed(){
	if(isJogging_)
		mRotationCtrl_->move(mRotationCtrl_->value()+jogStep_);
	else{
		/*
		//mRotationCtrl_->move(mRotationCtrl_->maximumValue());
		SGMBeamline::sgm()->ssaManipulatorRotVelocity()->move(3000);
		SGMBeamline::sgm()->ssaManipulatorRotVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorRotAcceleration()->move(2000);
		mRotationCtrl_->move(360.0);
		*/
		mRotationCtrl_->setVelocity(3000);
		mRotationCtrl_->setAcceleration(2000);
		mRotationCtrl_->move(360.0);
	}
}

void SGMSampleManipulatorView::onMCCWButtonReleased(){
	if(!isJogging_){
		mRotationCtrl_->stop();
		/*
		SGMBeamline::sgm()->ssaManipulatorRotVelocity()->move(10000);
		SGMBeamline::sgm()->ssaManipulatorRotVelocityBase()->move(2000);
		SGMBeamline::sgm()->ssaManipulatorRotAcceleration()->move(5000);
		//mRotationCtrl_->move(mRotationCtrl_->value());
		*/
		mRotationCtrl_->setVelocity(10000);
		mRotationCtrl_->setAcceleration(5000);
	}
}

void SGMSampleManipulatorView::onStopAllButtonClicked(){
	mVerticalCtrl_->stop();
	mHorizontalCtrl_->stop();
	mInPlaneCtrl_->stop();
	mRotationCtrl_->stop();
}

void SGMSampleManipulatorView::onJogButtonChecked(bool checked){
	isJogging_ = checked;
}

void SGMSampleManipulatorView::onJogSettingComboBoxChanged(int index){
	switch(index){
	case 0:
		jogStep_ = 0.1;
		break;
	case 1:
		jogStep_ = 0.25;
		break;
	case 2:
		jogStep_ = 0.5;
		break;
	case 3:
		jogStep_ = 1.0;
		break;
	}
}

void SGMSampleManipulatorView::onTransferPositionButtonClicked(){
	if(transferPositionActions_ && transferPositionActions_->isRunning())
		return;
	if(transferPositionActions_)
		delete transferPositionActions_;
	transferPositionActions_ = SGMBeamline::sgm()->createGoToTransferPositionActions();
	transferPositionActions_->start();
}

void SGMSampleManipulatorView::onMeasurePositionButtonClicked(){
	if(measurementPositionActions_ && measurementPositionActions_->isRunning())
		return;
	if(measurementPositionActions_)
		delete measurementPositionActions_;
	measurementPositionActions_ = SGMBeamline::sgm()->createGoToMeasurementPositionActions();
	measurementPositionActions_->start();
}

void SGMSampleManipulatorView::onHVButtonClicked(){
	if( SGMBeamline::sgm()->tfyHVToggle()->tolerance() > fabs(SGMBeamline::sgm()->tfyHVToggle()->value()-1) ){
		SGMBeamline::sgm()->tfyHVToggle()->move(0);
	}
	else{
		SGMBeamline::sgm()->tfyHVToggle()->move(1);
	}
}

void SGMSampleManipulatorView::onHVStateChanged(){
	double curHVValue = ((MCPDetector*)SGMBeamline::sgm()->tfyDetector())->hvCtrl()->value();
	switch( (int)(SGMBeamline::sgm()->tfyHVToggle()->value()) ){
	case 0:
		hvButton_->setText("HV is OFF");
		break;
	case 1:
		hvButton_->setText("HV is ON");
		break;
	case 2:
		hvButton_->setText(QString("HV is Ramping\nUp (%1)").arg(curHVValue, 0, 'f', 0));
		break;
	case 3:
		hvButton_->setText(QString("HV is Ramping\nDown (%1)").arg(curHVValue, 0, 'f', 0));
		break;
	}
	lastHVValue_ = curHVValue;
}

void SGMSampleManipulatorView::onIlluminatorSliderValueMoved(int newValue){
	SGMBeamline::sgm()->ssaIllumination()->move(newValue);
}

void SGMSampleManipulatorView::onIlluminatorFeedbackChanged(double newValue){
	if(!illuminatorSlider_->isSliderDown())
		illuminatorSlider_->setValue(newValue);
}

void SGMSampleManipulatorView::onIlluminatorPreset(int presetIndex){
	switch(presetIndex){
	case 0:
		SGMBeamline::sgm()->ssaIllumination()->move(0);
		break;
	case 1:
		SGMBeamline::sgm()->ssaIllumination()->move(10);
		break;
	case 2:
		SGMBeamline::sgm()->ssaIllumination()->move(25);
		break;
	case 3:
		SGMBeamline::sgm()->ssaIllumination()->move(100);
		break;
	}
}
