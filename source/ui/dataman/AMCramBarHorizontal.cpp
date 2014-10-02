/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMCramBarHorizontal.h"
#include "util/AMDeferredFunctionCall.h"
#include <QScrollBar>


 AMCramBarHorizontal::~AMCramBarHorizontal(){}
AMCramBarHorizontal::AMCramBarHorizontal(QWidget *parent) :
	QWidget(parent)
{
	parentWidget_ = 0;

	outerLayout_ = new QHBoxLayout();
	outerLayout_->setSpacing(0);
	outerLayout_->setContentsMargins(0,0,0,0);

	scrollLeftButton_ = new QToolButton();
	scrollLeftButton_->setText(QString(QChar(0x00AB)));// left "guillemet"
	scrollLeftButton_->setMaximumHeight(18);
	scrollLeftButton_->setAutoRepeat(true);
	scrollRightButton_ = new QToolButton();
	scrollRightButton_->setText(QString(QChar(0x00BB)));//right "guillemet"
	scrollRightButton_->setMaximumHeight(18);
	scrollRightButton_->setAutoRepeat(true);

	outerLayout_->addWidget(scrollLeftButton_);

	scrollArea_ = new AMScrollArea();
	scrollWidget_ = new QWidget();
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	innerLayout_ = new QHBoxLayout();
	innerLayout_->setSpacing(0);
	innerLayout_->setContentsMargins(0,0,0,0);
	innerLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
	scrollWidget_->setLayout(innerLayout_);
	scrollWidget_->installEventFilter(this);	// catches layout request events to detect changes to desired size.

	scrollArea_->setFrameStyle(QFrame::NoFrame);
	scrollArea_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	outerLayout_->addWidget(scrollArea_);
	outerLayout_->addWidget(scrollRightButton_);

	scrollArea_->setWidget(scrollWidget_);

	setLayout(outerLayout_);

	scrollLeftButton_->hide();
	scrollRightButton_->hide();
	scrollButtonsOn_ = false;

	connect(scrollLeftButton_, SIGNAL(clicked()), this, SLOT(onScrollButtonClicked()));
	connect(scrollRightButton_, SIGNAL(clicked()), this, SLOT(onScrollButtonClicked()));

	checkIfScrollButtonsRequired_ = new AMDeferredFunctionCall(this);
	connect(checkIfScrollButtonsRequired_, SIGNAL(executed()), this, SLOT(reviewScrollButtonsRequired()));
}

void AMCramBarHorizontal::onScrollButtonClicked() {
	if(sender() == scrollLeftButton_)
		scrollArea_->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);

	if(sender() == scrollRightButton_)
		scrollArea_->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}


bool AMCramBarHorizontal::eventFilter(QObject *source, QEvent *event) {
	if(source == scrollWidget_ && event->type() == QEvent::LayoutRequest) {
		scrollArea_->updateGeometry();	// notifies ourself and our outer layout that the scrollArea's sizeHint() has changed.
	}

	if(source == parentWidget_ && parentWidget_ && event->type() == QEvent::LayoutRequest) {
		checkIfScrollButtonsRequired_->schedule();
	}

	return QWidget::eventFilter(source, event);
}

bool AMCramBarHorizontal::event(QEvent* event)
{
	if(event->type() == QEvent::ParentAboutToChange) {
		if(parentWidget_) {
			parentWidget_->removeEventFilter(this);
			parentWidget_ = 0;
		}
	}

	if(event->type() == QEvent::ParentChange) {
		parentWidget_ = parentWidget();
		parentWidget_->installEventFilter(this);
	}

	return QWidget::event(event);
}

void AMCramBarHorizontal::reviewScrollButtonsRequired()
{
	if(scrollArea_->width() >= scrollWidget_->width()) {
		if(scrollButtonsOn_) {
			scrollLeftButton_->hide();
			scrollRightButton_->hide();
			scrollButtonsOn_ = false;
		}
	}
	else {
		if(!scrollButtonsOn_){
			scrollButtonsOn_ = true;
			scrollLeftButton_->show();
			scrollRightButton_->show();
		}
	}
}
 AMScrollArea::~AMScrollArea(){}
