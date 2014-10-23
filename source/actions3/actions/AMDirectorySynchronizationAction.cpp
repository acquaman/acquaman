#include "AMDirectorySynchronizationAction.h"

AMDirectorySynchronizationAction::AMDirectorySynchronizationAction(AMActionInfo3* info, QObject *parent) :
	AMAction3(info, parent)
{
		synchronizer_ = new AMDirectorySynchronizer(AMUserSettings::userDataFolder, AMUserSettings::remoteDataFolder);

	if(info){
		info->setShortDescription(QString("Data Backup"));
		info->setLongDescription(QString("Date Backup"));
	}
	setProgress(0, 100);
	setExpectedDuration(-1);
	setFailureResponseInActionRunner(AMAction3::PromptUserResponse);
	setFailureResponseAsSubAction(AMAction3::PromptUserResponse);

	connect(synchronizer_, SIGNAL(copyCompleted()), this, SLOT(onCopyCompleted()));
	connect(synchronizer_, SIGNAL(copyFailed()), this, SLOT(onCopyFailed()));
	connect(synchronizer_, SIGNAL(percentageProgressChanged(int)), this, SLOT(onCopyPercentageProgressChanged(int)));
	connect(synchronizer_, SIGNAL(progressMessagesChanged(const QString&)), this, SLOT(onCopyProgressMessageChanged(const QString&)));
}

void AMDirectorySynchronizationAction::startImplementation()
{

	if(info()){
		info()->setShortDescription(QString("Data Backup on %1").arg(QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate)));
		info()->setLongDescription(QString("Date Backup from %1 to %2 at %3").arg(synchronizer_->side1Directory()).arg(synchronizer_->side2Directory()).arg(QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate)));
	}
	if(!synchronizer_->start())
		setFailed();
}

void AMDirectorySynchronizationAction::pauseImplementation()
{
}

void AMDirectorySynchronizationAction::resumeImplementation()
{
}

void AMDirectorySynchronizationAction::cancelImplementation()
{
}

void AMDirectorySynchronizationAction::skipImplementation(const QString&)
{
}

bool AMDirectorySynchronizationAction::hasChildren() const
{
	return false;
}

int AMDirectorySynchronizationAction::numberOfChildren() const
{
	return 0;
}

void AMDirectorySynchronizationAction::onCopyCompleted()
{
	setSucceeded();
}

void AMDirectorySynchronizationAction::onCopyFailed()
{
	setFailed();
}

void AMDirectorySynchronizationAction::onCopyPercentageProgressChanged(int percentageProgress)
{
	setProgress(percentageProgress, 100);
}

void AMDirectorySynchronizationAction::onCopyProgressMessageChanged(const QString &message)
{
	if(message.length() > 35)
		setStatusText(QString("%1...").arg(message.mid(0, 32)));
	else
		setStatusText(message);
}
