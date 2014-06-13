#include "AMDeploy.h"

#include <QProcess>
#include <QCoreApplication>

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
	gitStatusOutput_.append(gitStatusProcess_->readAllStandardOutput());
	qDebug() << "Git status says: " << gitStatusOutput_;
}

void AMDeploy::onGitStatusProcessFinished(int status){
	qDebug() << "Git status exited with " << status;
	if(status == 0){
		gitBranchProcess_ = new QProcess();
		QString program = "git";
		QStringList arguments;
		arguments << "branch";
		gitBranchProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

		connect(gitBranchProcess_, SIGNAL(readyRead()), this, SLOT(onGitBranchProcessReadReady()));
		connect(gitBranchProcess_, SIGNAL(finished(int)), this, SLOT(onGitBranchProcessFinished(int)));
		gitBranchProcess_->start(program, arguments);
	}
}

void AMDeploy::onGitBranchProcessReadReady(){
	gitBranchOutput_.append(gitBranchProcess_->readAllStandardOutput());
	qDebug() << "Git branch says: " << gitBranchOutput_;
}

void AMDeploy::onGitBranchProcessFinished(int status){
	qDebug() << "Git branch exited with " << status;
	if(status == 0){
		gitLogProcess_ = new QProcess();
		QString program = "git";
		QStringList arguments;
		arguments << "log" << "-n" << "1";
		gitLogProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

		connect(gitLogProcess_, SIGNAL(readyRead()), this, SLOT(onGitLogProcessReadReady()));
		connect(gitLogProcess_, SIGNAL(finished(int)), this, SLOT(onGitLogProcessFinished(int)));
		gitLogProcess_->start(program, arguments);
	}
}

void AMDeploy::onGitLogProcessReadReady(){
	gitLogOutput_.append(gitLogProcess_->readAllStandardOutput());
	qDebug() << "Git log says: " << gitLogOutput_;
}

void AMDeploy::onGitLogProcessFinished(int status){
	qDebug() << "Git log exited with " << status;
	if(status == 0){
		gitDescribeProcess_ = new QProcess();
		QString program = "git";
		QStringList arguments;
		arguments << "describe";
		gitDescribeProcess_->setWorkingDirectory("/Users/chevrid/beamline/programming/acquaman");

		connect(gitDescribeProcess_, SIGNAL(readyRead()), this, SLOT(onGitDescribeProcessReadReady()));
		connect(gitDescribeProcess_, SIGNAL(finished(int)), this, SLOT(onGitDescribeProcessFinished(int)));
		gitDescribeProcess_->start(program, arguments);
	}
}

void AMDeploy::onGitDescribeProcessReadReady(){
	gitDescribeOutput_.append(gitDescribeProcess_->readAllStandardOutput());
	qDebug() << "Git describe says: " << gitDescribeOutput_;
}

void AMDeploy::onGitDescribeProcessFinished(int status){
	qDebug() << "Git describe exited with " << status;
	if(status == 0){
		qDebug() << "I think we've successfully done these steps";
		QCoreApplication::instance()->quit();
	}
}

AMDeploy::~AMDeploy()
{
}
