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
 	virtual ~AMCrashReporterUnixSignalHandler();
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
 	virtual ~AMCrashReporterStackTraceSymbol();
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
 	virtual ~AMCrashMonitor();
	AMCrashMonitor(const QString &executableFullPath, const QString &errorFilePath, int watchingPID, QObject *parent = 0);

protected slots:
	void onSiguser1Detected();
	void onSiguser2Detected();

	void onWatchingPIDTimerTimeout();
	void onCrashReporterPIDTimerTimeout();

	void onWatchingPIDFoundProcess(bool wasFound);
	void onCrashReporterPIDFoundProcess(bool wasFound);

protected:
	QString executableFullPath_;
	QString errorFilePath_;
	int watchingPID_;

	qint64 crashReporterPID_;

	AMCrashReporterUnixSignalHandler *unixSignalHandler_;

	QTimer *watchingPIDTimer_;
	QTimer *crashReporterPIDTimer_;
};

class AMCrashReporter : public QWidget
{
Q_OBJECT
public:
	AMCrashReporter(const QString &executableFullPath, const QString &errorFilePath, int watchingPID, int monitorPID, QWidget *parent = 0);
	
protected slots:
	void onSiguser1Detected();

	void onOneSymbolProcessed();

	void onAllSymbolsProcessed();
	void requestMonitorClose();

protected:
	virtual void closeEvent(QCloseEvent *e);

protected:
	QString executableFullPath_;
	QString errorFilePath_;
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

class AMProcessWatcher : public QObject
{
Q_OBJECT
public:
	AMProcessWatcher(qint64 watchPID, QObject *parent = 0);

public slots:
	void startWatching();

signals:
	void foundWatchPID(bool wasFound);

protected slots:
	void onProcessReadReady();
	void onProcessFinished();

protected:
	qint64 watchPID_;

	QProcess *psCommand_;
	bool foundProcess_;
};
