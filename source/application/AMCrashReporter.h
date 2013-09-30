#include <QWidget>
#include <QSocketNotifier>

#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

class QProcess;
class QProgressBar;
class QLabel;

class AMCrashReporterUnixSignalHandler : public QObject
{
Q_OBJECT
public:
	AMCrashReporterUnixSignalHandler(QObject *parent = 0);

	static void sigusr1SignalHandler(int signum);
	static void sigusr2SignalHandler(int signum);

public slots:
	void handleSigusr1();
	void handleSigusr2();

signals:
	void sigusr1Detected();
	void sigusr2Detected();

private:
	static int sigusr1Fd[2];
	QSocketNotifier *snSigusr1;

	static int sigusr2Fd[2];
	QSocketNotifier *snSigusr2;
};

class AMCrashReporterStackTraceSymbol : public QObject
{
Q_OBJECT
public:
	AMCrashReporterStackTraceSymbol(const QString &executable, const QString &address, QObject *parent = 0);

	QString executable() const { return executable_; }
	QString address() const { return address_; }

	bool isInvalid() const { return invalid_; }

public slots:
	void setInvalid(bool invalid) { invalid_ = invalid; }

protected:
	QString executable_;
	QString address_;

	bool invalid_;
};

class AMCrashMonitor : public QObject
{
Q_OBJECT
public:
	AMCrashMonitor(const QString &executableFullPath, int watchingPID, QObject *parent = 0);

protected slots:
	void onSiguser1Detected();
	void onSiguser2Detected();

protected:
	QString executableFullPath_;
	int watchingPID_;

	qint64 crashReporterPID_;

	AMCrashReporterUnixSignalHandler *unixSignalHandler_;
};

class AMCrashReporter : public QWidget
{
Q_OBJECT
public:
	AMCrashReporter(const QString &executableFullPath, int watchingPID, int monitorPID, QWidget *parent = 0);
	
protected slots:
	void onSiguser1Detected();

	void onOneSymbolProcessed();

	void onAllSymbolsProcessed();
	void requestMonitorClose();

protected:
	QString executableFullPath_;
	int watchingPID_;
	int monitorPID_;

	AMCrashReporterUnixSignalHandler *unixSignalHandler_;
	QList<AMCrashReporterStackTraceSymbol*> allSymbols_;
	QStringList allProcessedLines_;

	QProcess *activeAddressConversion_;

	QLabel *titleLabel_;
	QLabel *messageLabel_;
	QProgressBar *progressBar_;
};
