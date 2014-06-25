#include "AMCrashMonitorSupport.h"

#include <signal.h>
#include <execinfo.h>

#include <QApplication>
#include <QFileInfo>
#include <QProcess>
#include <QDir>

#include <QDebug>

qint64 globalCrashMonitorPID;
QFile *globalErrorFile;

AMCrashMonitorSupport* AMCrashMonitorSupport::crashMonitorSupportInstance_ = 0;

AMCrashMonitorSupport::AMCrashMonitorSupport()
{
	errorFile_ = 0;

	QCoreApplication *app = QApplication::instance();

	QStringList applicationArguments = app->arguments();

	QString applicationPath = applicationArguments.at(0);
	QFileInfo applicationPathInfo(applicationPath);

	if(applicationPathInfo.isSymLink())
		pathToCrashReporter_ = applicationPathInfo.symLinkTarget().section('/', 0, -2);
	else
		pathToCrashReporter_ = applicationPathInfo.absoluteDir().path();

	pathToCrashReportFiles_ = "/home/acquaman/AcquamanApplicationCrashReports";

	if(applicationArguments.contains("--enableCrashMonitor")){
		signal(SIGSEGV, handle_signal_sigsev);
		signal(SIGABRT, handle_signal_sigabrt);
	}
}

AMCrashMonitorSupport* AMCrashMonitorSupport::s(){
	if(!crashMonitorSupportInstance_)
		crashMonitorSupportInstance_ = new AMCrashMonitorSupport();
	return crashMonitorSupportInstance_;
}

QFile* AMCrashMonitorSupport::errorFile() const{
	return errorFile_;
}

QString AMCrashMonitorSupport::pathToCrashReporter() const{
	return pathToCrashReporter_;
}

QString AMCrashMonitorSupport::pathToCrashReportFiles() const{
	return pathToCrashReportFiles_;
}

void AMCrashMonitorSupport::setErrorFile(QFile *errorFile){
	if(!errorFile_){
		errorFile_ = errorFile;
		globalErrorFile = errorFile;
	}
}

void AMCrashMonitorSupport::setPathToCrashReporter(const QString &pathToCrashReporter){
	pathToCrashReporter_ = pathToCrashReporter;
}

void AMCrashMonitorSupport::setPathToCrashReportFiles(const QString &pathToCrashReportFiles){
	pathToCrashReportFiles_ = pathToCrashReportFiles;
}

void AMCrashMonitorSupport::monitor(){
	if(!errorFile_)
		return;

	QCoreApplication *app = QApplication::instance();

	QStringList appArguments = app->arguments();
	if(!appArguments.contains("--enableCrashMonitor"))
		return;

	QStringList arguments;
	arguments << "-m";
	arguments << app->applicationFilePath();
	arguments << pathToCrashReportFiles_;
	arguments << QString("%1").arg(getpid());

	QProcess::startDetached(pathToCrashReporter_+"/AMCrashReporter", arguments, QDir::currentPath(), &globalCrashMonitorPID);
}

void AMCrashMonitorSupport::report(){
	QCoreApplication *app = QApplication::instance();

	QStringList appArguments = app->arguments();
	if(!appArguments.contains("--enableCrashMonitor"))
		return;

	kill(globalCrashMonitorPID, SIGUSR2);
}

void handle_signal_sigsev(int signum){
	void *array[100];
	size_t size;

	size = backtrace(array, 100);
	backtrace_symbols_fd(array, size, globalErrorFile->handle());

	kill(globalCrashMonitorPID, SIGUSR1);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}

void handle_signal_sigabrt(int signum){
	void *array[100];
	size_t size;

	size = backtrace(array, 100);
	backtrace_symbols_fd(array, size, globalErrorFile->handle());
                
	kill(globalCrashMonitorPID, SIGUSR1);
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}
