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


#include "AMMessageBoxWTimeout.h"

#include <QTimer>

 AMMessageBoxWTimeout::~AMMessageBoxWTimeout(){}
AMMessageBoxWTimeout::AMMessageBoxWTimeout(int mSecTimeout, QWidget *parent) :
	QMessageBox(parent)
{
	mSecTimeout_ = mSecTimeout;
	timoutCountdown_ = mSecTimeout_;

	timeoutCountdownButton_ = new QPushButton();
	timeoutCountdownButton_->setEnabled(false);
}

int AMMessageBoxWTimeout::execWTimeout(){
	timeoutCountdownButton_->setText(QString("This message will close in %1").arg(timoutCountdown_/1000));
	addButton(timeoutCountdownButton_, QMessageBox::ResetRole);

	timeoutTimer_ = new QTimer();
	timeoutTimer_->setSingleShot(true);
	timeoutTimer_->setInterval(mSecTimeout_);
	connect(timeoutTimer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
	timeoutTimer_->start();

	timeoutCountdownTimer_ = new QTimer();
	timeoutCountdownTimer_->setInterval(1000);
	connect(timeoutCountdownTimer_, SIGNAL(timeout()), this, SLOT(onTimeoutCountdown()));
	timeoutCountdownTimer_->start();

	return QMessageBox::exec();
}

void AMMessageBoxWTimeout::onTimeout(){
	timeoutCountdownTimer_->stop();

	disconnect(timeoutCountdownTimer_, SIGNAL(timeout()), this, SLOT(onTimeoutCountdown()));
	disconnect(timeoutTimer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
	QMessageBox::defaultButton()->click();
}

void AMMessageBoxWTimeout::onTimeoutCountdown(){
	timoutCountdown_ = timoutCountdown_ - 1000;
	timeoutCountdownButton_->setText(QString("This message will close in %1").arg(timoutCountdown_/1000));
}

void AMMessageBoxWTimeout::closeEvent(QCloseEvent *event){
	if(timeoutTimer_->isActive()){
		disconnect(timeoutTimer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
		timeoutTimer_->stop();
	}
	if(timeoutCountdownTimer_->isActive()){
		disconnect(timeoutCountdownTimer_, SIGNAL(timeout()), this, SLOT(onTimeoutCountdown()));
		timeoutCountdownTimer_->stop();
	}
	QMessageBox::closeEvent(event);
}
