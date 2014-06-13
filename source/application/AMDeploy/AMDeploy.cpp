#include "AMDeploy.h"

#include <QProcess>

#include <QDebug>

AMDeploy::AMDeploy(QObject *parent) :
	QObject(parent)
{
	gitStatusProcess_ = new QProcess();
	QString program = "git";
	QStringList arguments;
	arguments << "status" << "--short";
	gitStatusProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

	connect(gitStatusProcess_, SIGNAL(readyRead()), this, SLOT(onGitStatusProcessReadReady()));
	connect(gitStatusProcess_, SIGNAL(finished(int)), this, SLOT(onGitStatusProcessFinished(int)));
	gitStatusProcess_->start(program, arguments);
}

void AMDeploy::onGitStatusProcessReadReady(){
	qDebug() << "Git status says: " << gitStatusProcess_->readAll();
}

void AMDeploy::onGitStatusProcessFinished(int status){
	qDebug() << "Git status exited with " << status;
}

AMDeploy::~AMDeploy()
{
}
