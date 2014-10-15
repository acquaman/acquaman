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
	arguments << QString("%1").arg(getpt());

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
	kill(getpt(), signum);
}

void handle_signal_sigabrt(int signum){
	void *array[100];
	size_t size;

	size = backtrace(array, 100);
	backtrace_symbols_fd(array, size, globalErrorFile->handle());
                
	kill(globalCrashMonitorPID, SIGUSR1);
	signal(signum, SIG_DFL);
	kill(getpt(), signum);
}
