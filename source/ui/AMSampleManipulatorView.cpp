#include "AMSampleManipulatorView.h"

AMSampleManipulatorView::AMSampleManipulatorView(QString title, AMControlSet *manipulator, QWidget *parent) :
	QGroupBox(title, parent)
{
	manipulator_ = manipulator;
}

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

	mVerticalCtrl_ = manipulator_->controlByName("ssaManipulatorZ");
	mHorizontalCtrl_ = manipulator_->controlByName("ssaManipulatorX");
	mInPlaneCtrl_ = manipulator_->controlByName("ssaManipulatorY");

	mVerticalNC_ = new NumericControl(mVerticalCtrl_);
	mVerticalNC_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	mVerticalNC_->setMinimumWidth(92);
	mHorizontalNC_ = new NumericControl(mHorizontalCtrl_);
	mHorizontalNC_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	mHorizontalNC_->setMinimumWidth(92);
	mInPlaneNC_ = new NumericControl(mInPlaneCtrl_);
	mInPlaneNC_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	mInPlaneNC_->setMinimumWidth(92);

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

	gl_ = new QGridLayout();
	gl_->addWidget(mUpButton_,			0, 1, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mDownButton_,		2, 1, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mInboardButton_,		1, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mOutboardButton_,	1, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mUpstreamButton_,	2, 0, 1, 1, Qt::AlignCenter);
	gl_->addWidget(mDownstreamButton_,	0, 2, 1, 1, Qt::AlignCenter);
	gl_->addWidget(new QLabel("Vertical"),		0, 3, 1, 1, Qt::AlignJustify);
	gl_->addWidget(new QLabel("Horizontal"),		1, 3, 1, 1, Qt::AlignJustify);
	gl_->addWidget(new QLabel("In Plane"),			2, 3, 1, 1, Qt::AlignJustify);
	gl_->addWidget(mVerticalNC_,		0, 4, 1, 1, Qt::AlignLeft);
	gl_->addWidget(mHorizontalNC_,		1, 4, 1, 1, Qt::AlignLeft);
	gl_->addWidget(mInPlaneNC_,			2, 4, 1, 1, Qt::AlignLeft);

	setLayout(gl_);
}

void SGMSampleManipulatorView::onMUpButtonPressed(){
	mVerticalCtrl_->move(mVerticalCtrl_->maximumValue());
}

void SGMSampleManipulatorView::onMUpButtonReleased(){
	mVerticalCtrl_->stop();
}

void SGMSampleManipulatorView::onMDownButtonPressed(){
	mVerticalCtrl_->move(mVerticalCtrl_->minimumValue());
}

void SGMSampleManipulatorView::onMDownButtonReleased(){
	mVerticalCtrl_->stop();
}

void SGMSampleManipulatorView::onMInboardButtonPressed(){
	mHorizontalCtrl_->move(mHorizontalCtrl_->maximumValue());
}

void SGMSampleManipulatorView::onMInboardButtonReleased(){
	mHorizontalCtrl_->stop();
	mHorizontalCtrl_->move(mHorizontalCtrl_->value());
}

void SGMSampleManipulatorView::onMOutboardButtonPressed(){
	mHorizontalCtrl_->move(mHorizontalCtrl_->minimumValue());
}

void SGMSampleManipulatorView::onMOutboardButtonReleased(){
	mHorizontalCtrl_->stop();
}

void SGMSampleManipulatorView::onMUpstreamButtonPressed(){
	mInPlaneCtrl_->move(mInPlaneCtrl_->maximumValue());
}

void SGMSampleManipulatorView::onMUpstreamButtonReleased(){
	mInPlaneCtrl_->stop();
}

void SGMSampleManipulatorView::onMDownstreamButtonPressed(){
	mInPlaneCtrl_->move(mInPlaneCtrl_->minimumValue());
}

void SGMSampleManipulatorView::onMDownstreamButtonReleased(){
	mInPlaneCtrl_->stop();
}
