#ifndef AMCRASHMONITORSUPPORT_H
#define AMCRASHMONITORSUPPORT_H

#include <QFile>

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

void handle_signal(int signum);

#endif // AMCRASHMONITORSUPPORT_H
