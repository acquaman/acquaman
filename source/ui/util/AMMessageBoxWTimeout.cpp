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
