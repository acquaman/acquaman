/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMSampleManipulatorView.h"

AMSampleManipulatorView::AMSampleManipulatorView(QString title, AMControlSet *manipulator, QWidget *parent) :
	QGroupBox(title, parent)
{
	manipulator_ = manipulator;
}

AMControlSet* AMSampleManipulatorView::manipulator(){
	return manipulator_;
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
	cwIcon_ = QIcon(":/ArrowCW.png");
	mCWButton_ = new QPushButton(cwIcon_, "");
	mCWButton_->setToolTip("Move Clockwise");
	ccwIcon_ = QIcon(":/ArrowCCW.png");
	mCCWButton_ = new QPushButton(ccwIcon_, "");
	mCCWButton_->setToolTip("Move Counter-Clockwise");

	mVerticalCtrl_ = manipulator_->controlByName("ssaManipulatorZ");
	mHorizontalCtrl_ = manipulator_->controlByName("ssaManipulatorX");
	mInPlaneCtrl_ = manipulator_->controlByName("ssaManipulatorY");
	mRotationCtrl_ = manipulator_->controlByName("ssaManipulatorRot");

	mVerticalNC_ = new AMNumericControl(mVerticalCtrl_);
	mVerticalNC_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	mVerticalNC_->setMinimumSize(92, 24);
	mHorizontalNC_ = new AMNumericControl(mHorizontalCtrl_);
	mHorizontalNC_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	mHorizontalNC_->setMinimumSize(92, 24);
	mInPlaneNC_ = new AMNumericControl(mInPlaneCtrl_);
	mInPlaneNC_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	mInPlaneNC_->setMinimumSize(92, 24);
	mRotationNC_ = new AMNumericControl(mRotationCtrl_);
	mRotationNC_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	mRotationNC_->setMinimumSize(92, 24);

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

	gl_ = new QGridLayout();
	gl_->addWidget(mUpButton_,			0, 1, 4, 1, Qt::AlignCenter);
	gl_->addWidget(mDownButton_,		8, 1, 4, 1, Qt::AlignCenter);
	gl_->addWidget(mInboardButton_,		4, 0, 4, 1, Qt::AlignCenter);
	gl_->addWidget(mOutboardButton_,	4, 2, 4, 1, Qt::AlignCenter);
	gl_->addWidget(mUpstreamButton_,	8, 0, 4, 1, Qt::AlignCenter);
	gl_->addWidget(mDownstreamButton_,	0, 2, 4, 1, Qt::AlignCenter);
	gl_->addWidget(mCWButton_,			0, 0, 4, 1, Qt::AlignCenter);
	gl_->addWidget(mCCWButton_,			8, 2, 4, 1, Qt::AlignCenter);
	gl_->addWidget(new QLabel("Vertical"),			0, 3, 3, 1, Qt::AlignJustify|Qt::AlignVCenter);
	gl_->addWidget(new QLabel("Horizontal"),		3, 3, 3, 1, Qt::AlignJustify|Qt::AlignVCenter);
	gl_->addWidget(new QLabel("In Plane"),			6, 3, 3, 1, Qt::AlignJustify|Qt::AlignVCenter);
	gl_->addWidget(new QLabel("Rotation"),			9, 3, 3, 1, Qt::AlignJustify|Qt::AlignVCenter);
	gl_->addWidget(mVerticalNC_,		0, 4, 3, 1, Qt::AlignLeft|Qt::AlignVCenter);
	gl_->addWidget(mHorizontalNC_,		3, 4, 3, 1, Qt::AlignLeft|Qt::AlignVCenter);
	gl_->addWidget(mInPlaneNC_,			6, 4, 3, 1, Qt::AlignLeft|Qt::AlignVCenter);
	gl_->addWidget(mRotationNC_,		9, 4, 3, 1, Qt::AlignLeft|Qt::AlignVCenter);

	setLayout(gl_);
}

void SGMSampleManipulatorView::onMUpButtonPressed(){
	mVerticalCtrl_->move(mVerticalCtrl_->maximumValue());
}

void SGMSampleManipulatorView::onMUpButtonReleased(){
	mVerticalCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorZStop()->move(0);
}

void SGMSampleManipulatorView::onMDownButtonPressed(){
	mVerticalCtrl_->move(mVerticalCtrl_->minimumValue());
}

void SGMSampleManipulatorView::onMDownButtonReleased(){
	mVerticalCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorZStop()->move(0);
}

void SGMSampleManipulatorView::onMInboardButtonPressed(){
	mHorizontalCtrl_->move(mHorizontalCtrl_->maximumValue());
}

void SGMSampleManipulatorView::onMInboardButtonReleased(){
	mHorizontalCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorXStop()->move(0);
}

void SGMSampleManipulatorView::onMOutboardButtonPressed(){
	mHorizontalCtrl_->move(mHorizontalCtrl_->minimumValue());
}

void SGMSampleManipulatorView::onMOutboardButtonReleased(){
	mHorizontalCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorXStop()->move(0);
}

void SGMSampleManipulatorView::onMUpstreamButtonPressed(){
	mInPlaneCtrl_->move(mInPlaneCtrl_->maximumValue());
}

void SGMSampleManipulatorView::onMUpstreamButtonReleased(){
	mInPlaneCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorYStop()->move(0);
}

void SGMSampleManipulatorView::onMDownstreamButtonPressed(){
	mInPlaneCtrl_->move(mInPlaneCtrl_->minimumValue());
}

void SGMSampleManipulatorView::onMDownstreamButtonReleased(){
	mInPlaneCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorYStop()->move(0);
}

void SGMSampleManipulatorView::onMCWButtonPressed(){
	mRotationCtrl_->move(mRotationCtrl_->minimumValue());
}

void SGMSampleManipulatorView::onMCWButtonReleased(){
	mRotationCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorRotStop()->move(0);
}

void SGMSampleManipulatorView::onMCCWButtonPressed(){
	mRotationCtrl_->move(mRotationCtrl_->maximumValue());
}

void SGMSampleManipulatorView::onMCCWButtonReleased(){
	mRotationCtrl_->stop();
	SGMBeamline::sgm()->ssaManipulatorRotStop()->move(0);
}
