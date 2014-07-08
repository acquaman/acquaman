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


#include <QWidget>
#include <QSocketNotifier>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

class QProcess;
class QProgressBar;
class QLabel;
class QCloseEvent;

class AMCrashReporterUnixSignalHandler : public QObject
{
Q_OBJECT
public:
	/// Convenience class to set up unix signal handlers
	AMCrashReporterUnixSignalHandler(QObject *parent = 0);
	virtual ~AMCrashReporterUnixSignalHandler();

	/// Function for handling write on SIGUSR1
	static void sigusr1SignalHandler(int signum);
	/// Function for handling write on SIGUSR2
	static void sigusr2SignalHandler(int signum);

public slots:
	/// Function for handling read on SIGUSR1
	void handleSigusr1();
	/// Function for handling read on SIGUSR2
	void handleSigusr2();

signals:
	/// Emitted when SIGUSR1 is detected
	void sigusr1Detected();
	/// Emitted when SIGUSR2 is detected
	void sigusr2Detected();

private:
	/// Unix file descriptor for SIGSUR1
	static int sigusr1Fd[2];
	/// Socket notifier for SIGUSR1
	QSocketNotifier *snSigusr1;

	/// Unix file descriptor for SIGSUR2
	static int sigusr2Fd[2];
	/// Socket notifier for SIGUSR2
	QSocketNotifier *snSigusr2;
};

class AMCrashReporterStackTraceSymbol : public QObject
{
Q_OBJECT
public:
	/// Simple mapping class for stack trace symbols
	AMCrashReporterStackTraceSymbol(const QString &executable, const QString &address, QObject *parent = 0);
	virtual ~AMCrashReporterStackTraceSymbol();

	/// Returns the executable name
	QString executable() const { return executable_; }
	/// Returns the address
	QString address() const { return address_; }

	/// Returns validity flag
	bool isInvalid() const { return invalid_; }

public slots:
	/// Sets validity flag
	void setInvalid(bool invalid) { invalid_ = invalid; }

protected:
	/// Holds executable name
	QString executable_;
	/// Hold address
	QString address_;

	/// Hold validity flag
	bool invalid_;
};

class AMCrashMonitor : public QObject
{
Q_OBJECT
public:
	/// Class to run the crash monitor - long lived program that watches an application, and launches a crash reporter which parses SIGSEV output
	AMCrashMonitor(const QString &executableFullPath, const QString &errorFilePath, int watchingPID, QObject *parent = 0);
	virtual ~AMCrashMonitor();

protected slots:
	/// Handles SIGUSR1, which is a signal from an Acquaman application basically saying that it SIGSEV'd. This causes the crash reporter to run and parse the back trace.
	void onSiguser1Detected();
	/// Handles SIGUSR2, which is a signal from the crash reporter saying that it ran to completion
	void onSiguser2Detected();

	/// Handles a timer's timeout to make sure that the crash monitor didn't miss the Acquaman application dying, calls a AMProcessWatcher to do this
	void onWatchingPIDTimerTimeout();
	/// Handles a timer's timeout to make sure that the crash monitor didn't miss the crash reporter closing, calls a AMProcessWatcher to do this
	void onCrashReporterPIDTimerTimeout();

	/// Handles output from an AMProcessWatcher signal, if the process for the Acquaman application wasn't found then the crash monitor dies gracefully
	void onWatchingPIDFoundProcess(bool wasFound);
	/// Handles output from an AMProcessWatcher signal, if the process for the crash reporter wasn't found then the crash monitor dies gracefully
	void onCrashReporterPIDFoundProcess(bool wasFound);

protected:
	/// Holds the Acquaman application full file path (used to find the name of the application)
	QString executableFullPath_;
	/// Holds the path to the temporary error file (back trace) that should be generated
	QString errorFilePath_;
	/// The PID of the Acquaman application
	int watchingPID_;

	/// The PID of the crash reporter
	qint64 crashReporterPID_;

	/// Our signal handler class
	AMCrashReporterUnixSignalHandler *unixSignalHandler_;

	/// Timer that goes off every 5 seconds to make sure that we didn't miss an Acquaman application dying
	QTimer *watchingPIDTimer_;
	/// Timer that goes off every 5 seconds after an Acquaman application crash to make sure we didn't miss the crash reporter closing
	QTimer *crashReporterPIDTimer_;
};

class AMCrashReporter : public QWidget
{
Q_OBJECT
public:
	/// The crash reporter launches a window to let the user know we're trying to generate a crash report and actually parses the stack trace
	AMCrashReporter(const QString &executableFullPath, const QString &errorFilePath, int watchingPID, int monitorPID, QWidget *parent = 0);
	
protected slots:
	/// Handles SIGUSR1, which is a message from the crash monitor to start parsing
	void onSiguser1Detected();

	/// Parses one symbol in the stack trace using addr2line or calls onAllSymbolsProcessed if it's done
	void onOneSymbolProcessed();

	/// Writes a readable stack trace file once all of the parsing and processing is done
	void onAllSymbolsProcessed();
	/// Sends off SIGUSR2 to close the crash monitor
	void requestMonitorClose();

protected:
	/// Reimplementation of the close event. We want to stop users from closing the window prematurely
	virtual void closeEvent(QCloseEvent *e);

protected:
	/// Holds the Acquaman application full file path (used to find the name of the application)
	QString executableFullPath_;
	/// Holds the path to the temporary error file (back trace) that should be generated
	QString errorFilePath_;
	/// The PID of the Acquaman application
	int watchingPID_;
	/// The PID of the crash monitor
	int monitorPID_;

	/// Our signal handler class
	AMCrashReporterUnixSignalHandler *unixSignalHandler_;
	/// A list of the stack trace symbols we parsed
	QList<AMCrashReporterStackTraceSymbol*> allSymbols_;
	/// Stringlist of the processed lines
	QStringList allProcessedLines_;

	/// Process that handles the add2line conversion
	QProcess *activeAddressConversion_;

	/// Window title
	QLabel *titleLabel_;
	/// Message label in the window to update user on the status of the report
	QLabel *messageLabel_;
	/// Progess bar for updating status
	QProgressBar *progressBar_;

	/// If we successfully processed the stack trace, then we can treat the closeEvent like a normal one
	bool successfullyProcessed_;
};

class AMProcessWatcher : public QObject
{
Q_OBJECT
public:
	/// Small class to look for a PID using a call to ps
	AMProcessWatcher(qint64 watchPID, QObject *parent = 0);

public slots:
	/// Starts the ps process
	void startWatching();

signals:
	/// Emits whether we found that PID or not
	void foundWatchPID(bool wasFound);

protected slots:
	/// Handles the standard output of the ps command
	void onProcessReadReady();
	/// Let's us know when we're done
	void onProcessFinished();

protected:
	/// PID we want to look for
	qint64 watchPID_;

	/// The ps process
	QProcess *psCommand_;
	/// Boolean that holds whether we found the PID or not
	bool foundProcess_;
};
