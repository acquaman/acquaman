#include "SGMSampleManipulatorView.h"
#include "beamline/SGM/SGMBeamline.h"

#include <QLabel>
#include <QPalette>
#include <QButtonGroup>

#include "beamline/AMBeamlineListAction.h"

SGMSampleManipulatorView::SGMSampleManipulatorView(QWidget *parent) :
		AMSampleManipulatorView("SGM SSA Endstation", SGMBeamline::sgm()->ssaManipulatorSet(), parent )
{
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

	transferPositionButton_ = new QPushButton("Transfer Position");
	transferPositionActions_ = 0;//NULL
	measurePositionButton_ = new QPushButton("Measure Position");
	measurementPositionActions_ = 0;//NULL

	mVerticalCtrl_ = manipulator_->controlNamed("ssaManipulatorZ");
	mHorizontalCtrl_ = manipulator_->controlNamed("ssaManipulatorX");
	mInPlaneCtrl_ = manipulator_->controlNamed("ssaManipulatorY");
	mRotationCtrl_ = manipulator_->controlNamed("ssaManipulatorRot");

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
	connect(transferPositionButton_, SIGNAL(clicked()), this, SLOT(onTransferPositionButtonClicked()));
	connect(measurePositionButton_, SIGNAL(clicked()), this, SLOT(onMeasurePositionButtonClicked()));
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
	vl2->addWidget(illuminatorSlider_);
	QLabel *illuminatorLabel = new QLabel("Chamber Light");
	illuminatorLabel->setAlignment(Qt::AlignCenter);
	vl2->addWidget(illuminatorLabel);
	vl2->addLayout(ipl);
	vl2->addWidget(transferPositionButton_);
	vl2->addWidget(measurePositionButton_);

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
	//mVerticalCtrl_->move(mVerticalCtrl_->maximumValue());
	mVerticalCtrl_->move(40);
}

void SGMSampleManipulatorView::onMUpButtonReleased(){
	mVerticalCtrl_->stop();
	//mVerticalCtrl_->move(mVerticalCtrl_->value());
}

void SGMSampleManipulatorView::onMDownButtonPressed(){
	//mVerticalCtrl_->move(mVerticalCtrl_->minimumValue());
	mVerticalCtrl_->move(-80);
}

void SGMSampleManipulatorView::onMDownButtonReleased(){
	mVerticalCtrl_->stop();
	//mVerticalCtrl_->move(mVerticalCtrl_->value());
}

void SGMSampleManipulatorView::onMInboardButtonPressed(){
	//mHorizontalCtrl_->move(mHorizontalCtrl_->maximumValue());
	mHorizontalCtrl_->move(10.0);
}

void SGMSampleManipulatorView::onMInboardButtonReleased(){
	mHorizontalCtrl_->stop();
	//mHorizontalCtrl_->move(mHorizontalCtrl_->value());
}

void SGMSampleManipulatorView::onMOutboardButtonPressed(){
	//mHorizontalCtrl_->move(mHorizontalCtrl_->minimumValue());
	mHorizontalCtrl_->move(-10.0);
}

void SGMSampleManipulatorView::onMOutboardButtonReleased(){
	mHorizontalCtrl_->stop();
	//mHorizontalCtrl_->move(mHorizontalCtrl_->value());
}

void SGMSampleManipulatorView::onMUpstreamButtonPressed(){
	//mInPlaneCtrl_->move(mInPlaneCtrl_->maximumValue());
	mInPlaneCtrl_->move(10.0);
}

void SGMSampleManipulatorView::onMUpstreamButtonReleased(){
	mInPlaneCtrl_->stop();
	//mInPlaneCtrl_->move(mInPlaneCtrl_->value());
}

void SGMSampleManipulatorView::onMDownstreamButtonPressed(){
	//mInPlaneCtrl_->move(mInPlaneCtrl_->minimumValue());
	mInPlaneCtrl_->move(-10.0);
}

void SGMSampleManipulatorView::onMDownstreamButtonReleased(){
	mInPlaneCtrl_->stop();
	//mInPlaneCtrl_->move(mInPlaneCtrl_->value());
}

void SGMSampleManipulatorView::onMCWButtonPressed(){
	//mRotationCtrl_->move(mRotationCtrl_->minimumValue());
	mRotationCtrl_->move(-360.0);
}

void SGMSampleManipulatorView::onMCWButtonReleased(){
	mRotationCtrl_->stop();
	//mRotationCtrl_->move(mRotationCtrl_->value());
}

void SGMSampleManipulatorView::onMCCWButtonPressed(){
	//mRotationCtrl_->move(mRotationCtrl_->maximumValue());
	mRotationCtrl_->move(360.0);
}

void SGMSampleManipulatorView::onMCCWButtonReleased(){
	mRotationCtrl_->stop();
	//mRotationCtrl_->move(mRotationCtrl_->value());
}

void SGMSampleManipulatorView::onStopAllButtonClicked(){
	mVerticalCtrl_->stop();
	mHorizontalCtrl_->stop();
	mInPlaneCtrl_->stop();
	mRotationCtrl_->stop();
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
