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


#ifndef AMCRASHMONITORSUPPORT_H
#define AMCRASHMONITORSUPPORT_H

#include <QFile>
#include <unistd.h>

class AMCrashMonitorSupport
{
public:
	/// Singleton accessor for the crash monitor support system
	static AMCrashMonitorSupport* s();

	/// The pointer to the error file we're going to use
	QFile* errorFile() const;
	/// Path to AMCrashReporter application if it's non-standard
	QString pathToCrashReporter() const;
	/// Path to write reports to (defaults to the AcquamanApplicationCrashReports directory in the acquaman account)
	QString pathToCrashReportFiles() const;

	/// We NEED to set the file before calling monitor and the file HAS to have the scope of the main.cpp main() call. Don't call this again after calling it once, it will do nothing
	void setErrorFile(QFile *errorFile);
	/// Set the path to AMCrashReporter application if it's non-standard
	void setPathToCrashReporter(const QString &pathToCrashReporter);
	/// Set the path to place reports in
	void setPathToCrashReportFiles(const QString &pathToCrashReportFiles);

	/// Call this to start the crash monitor system
	void monitor();
	/// Call this after the appController is shutdown to nicely close the crash monitor system
	void report();

protected:
	/// Protected constructor generates default values for all but errorFile_
	AMCrashMonitorSupport();

	/// Pointer to the error file
	QFile *errorFile_;	
	/// Path to the application
	QString pathToCrashReporter_;
	/// Path to place reports in
	QString pathToCrashReportFiles_;

private:
	/// Singleton instance
	static AMCrashMonitorSupport *crashMonitorSupportInstance_;
};

void handle_signal_sigsev(int signum);
void handle_signal_sigabrt(int signum);

#endif // AMCRASHMONITORSUPPORT_H
