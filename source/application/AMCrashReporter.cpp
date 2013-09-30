#include "AMCrashReporter.h"

#include <QDebug>
#include <QLabel>
#include <QBoxLayout>
#include <QDir>
#include <QProcess>
#include <QProgressBar>
#include <QTimer>
#include <QApplication>
#include <QDateTime>
#include <QHostInfo>
#include <QDesktopWidget>

int AMCrashReporterUnixSignalHandler::sigusr1Fd[2];
int AMCrashReporterUnixSignalHandler::sigusr2Fd[2];

AMCrashReporterUnixSignalHandler::AMCrashReporterUnixSignalHandler(QObject *parent) :
	QObject(parent)
{
	if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigusr1Fd))
		qFatal("Couldn't create SIGUSR1 socketpair");

	snSigusr1 = new QSocketNotifier(sigusr1Fd[1], QSocketNotifier::Read, this);
	connect(snSigusr1, SIGNAL(activated(int)), this, SLOT(handleSigusr1()));

	if(::socketpair(AF_UNIX, SOCK_STREAM, 0, sigusr2Fd))
		qFatal("Couldn't create SIGUSR2 socketpair");

	snSigusr2 = new QSocketNotifier(sigusr2Fd[1], QSocketNotifier::Read, this);
	connect(snSigusr2, SIGNAL(activated(int)), this, SLOT(handleSigusr2()));
}

void AMCrashReporterUnixSignalHandler::handleSigusr1(){
	snSigusr1->setEnabled(false);

	char tmp;
	ssize_t retVal = ::read(sigusr1Fd[1], &tmp, sizeof(tmp));
	Q_UNUSED(retVal);

	emit sigusr1Detected();

	snSigusr1->setEnabled(true);
}

void AMCrashReporterUnixSignalHandler::handleSigusr2(){
	snSigusr2->setEnabled(false);

	char tmp;
	ssize_t retVal = ::read(sigusr2Fd[1], &tmp, sizeof(tmp));
	Q_UNUSED(retVal);

	emit sigusr2Detected();

	snSigusr2->setEnabled(true);
}

void AMCrashReporterUnixSignalHandler::sigusr1SignalHandler(int signum){
	char a = 1;
	ssize_t retVal = ::write(sigusr1Fd[0], &a, sizeof(a));
	Q_UNUSED(retVal);
}

void AMCrashReporterUnixSignalHandler::sigusr2SignalHandler(int signum){
	char a = 1;
	ssize_t retVal = ::write(sigusr2Fd[0], &a, sizeof(a));
	Q_UNUSED(retVal);
}

AMCrashReporterStackTraceSymbol::AMCrashReporterStackTraceSymbol(const QString &executable, const QString &address, QObject *parent) :
	QObject(parent)
{
	executable_ = executable;
	address_ = address;
	invalid_ = false;
}

AMCrashMonitor::AMCrashMonitor(const QString &executableFullPath, int watchingPID, QObject *parent) :
	QObject(parent)
{
	executableFullPath_ = executableFullPath;
	watchingPID_ = watchingPID;

	unixSignalHandler_ = new AMCrashReporterUnixSignalHandler();
	connect(unixSignalHandler_, SIGNAL(sigusr1Detected()), this, SLOT(onSiguser1Detected()));
	connect(unixSignalHandler_, SIGNAL(sigusr2Detected()), this, SLOT(onSiguser2Detected()));
}

void AMCrashMonitor::onSiguser1Detected(){
	//qDebug() << "Detected SIGUSR1 in AMCrashMonitor, need to launch AMCrashReporter";

	QStringList arguments;
	arguments << executableFullPath_;
	arguments << QString("%1").arg(watchingPID_);
	arguments << QString("%1").arg(getpid());
	QProcess::startDetached(QCoreApplication::instance()->arguments().at(0), arguments, QDir::currentPath(), &crashReporterPID_);
}

void AMCrashMonitor::onSiguser2Detected(){
	//qDebug() << "Detected SIGUSR2 in AMCrashMonitor, need to close this program now";

	QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
}

AMCrashReporter::AMCrashReporter(const QString &executableFullPath, int watchingPID, int monitorPID, QWidget *parent) :
	QWidget(parent)
{
	executableFullPath_ = executableFullPath;
	watchingPID_ = watchingPID;
	monitorPID_ = monitorPID;
	activeAddressConversion_ = 0;

	unixSignalHandler_ = new AMCrashReporterUnixSignalHandler();
	connect(unixSignalHandler_, SIGNAL(sigusr1Detected()), this, SLOT(onSiguser1Detected()));


	titleLabel_ = new QLabel("This is embarassing, it looks like Acquaman has crashed");
	QFont titleFont = titleLabel_->font();
	titleFont.setBold(true);
	titleFont.setPointSizeF(titleFont.pointSizeF()*1.5);
	titleLabel_->setFont(titleFont);
	messageLabel_ = new QLabel("");
	messageLabel_->hide();
	progressBar_ = new QProgressBar();
	progressBar_->setRange(0, 1);
	progressBar_->setValue(0);
	progressBar_->setMaximumWidth(200);
	progressBar_->hide();
	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(titleLabel_);
	vl->addWidget(messageLabel_);
	vl->addWidget(progressBar_);
	setLayout(vl);

	setFixedSize(600, 400);

	QDesktopWidget *desktopWidget = QApplication::desktop();
	QRect screenGeometry = desktopWidget->availableGeometry();
	int xPositiion = (screenGeometry.width()/3)-300 + screenGeometry.x();
	if(xPositiion < 0)
		xPositiion = 0;
	int yPosition = (screenGeometry.height()/2)-200 + screenGeometry.y();
	if(yPosition < 0)
		yPosition = 0;
	//qDebug() << "Going to move to " << xPositiion << yPosition << desktopWidget->screenCount();
	move(xPositiion, yPosition);

	QTimer::singleShot(100, this, SLOT(onSiguser1Detected()));
}

void AMCrashReporter::onSiguser1Detected(){
	//qDebug() << "Detected SIGUSR1 in AMCrashReporter, need to check on PID " << watchingPID_;

	//qDebug() << "Does the error file exist?" << QFile::exists("/tmp/ErrorFile.txt");

	QString errorString;
	QString executableString;
	QString addressString;
	AMCrashReporterStackTraceSymbol *oneSymbol;
	QFile errorFile("/tmp/ErrorFile.txt");
	if(errorFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		while(!errorFile.atEnd()){
			errorString = errorFile.readLine();
			executableString = errorString.section('[', 0, 0);
			addressString = errorString.section('[', 1, 1).remove(']');

			oneSymbol = new AMCrashReporterStackTraceSymbol(executableString, addressString);
			if(executableString.contains('('))
				oneSymbol->setInvalid(true);
			allSymbols_.append(oneSymbol);
		}

		errorFile.close();
		errorFile.remove();

		messageLabel_->show();
		messageLabel_->setText("\nAcquaman is trying to process what happened.\nPlease wait while a report is generated.");
		progressBar_->show();
		progressBar_->setRange(0, 0);
		onOneSymbolProcessed();
	}
}

void AMCrashReporter::onOneSymbolProcessed(){
	if(activeAddressConversion_){
		disconnect(activeAddressConversion_, SIGNAL(finished(int)), this, SLOT(onOneSymbolProcessed()));

		QString allStandardOutput = activeAddressConversion_->readAllStandardOutput();
		//qDebug() << "Got an line number as " << allStandardOutput << "\n";

		allProcessedLines_.append(allStandardOutput);
	}

	if(allSymbols_.isEmpty()){
		progressBar_->setRange(0, 1);
		progressBar_->setValue(1);

		QTimer::singleShot(1000, this, SLOT(onAllSymbolsProcessed()));
	}
	else{
		AMCrashReporterStackTraceSymbol *oneSymbol = allSymbols_.takeFirst();
		if(oneSymbol->isInvalid()){
			activeAddressConversion_ = 0;
			onOneSymbolProcessed();
		}
		else{
			activeAddressConversion_ = new QProcess();
			QString program = "addr2line";
			QStringList arguments;
			//qDebug() << "Arguments: " << oneSymbol->executable() << oneSymbol->address();
			arguments << "-e" << oneSymbol->executable() << oneSymbol->address();
			activeAddressConversion_->start(program, arguments);

			connect(activeAddressConversion_, SIGNAL(finished(int)), this, SLOT(onOneSymbolProcessed()));
		}
	}
}

void AMCrashReporter::onAllSymbolsProcessed(){
	//qDebug() << "Processed as: ";
	QFile reportFile(QString("/home/acquaman/AcquamanApplicationCrashReports/report_%1_%2_%3.txt").arg(QHostInfo::localHostName()).arg(QDateTime::currentDateTime().toString("hhmmss_ddMMyyyy")).arg(watchingPID_));
	if(reportFile.open(QIODevice::WriteOnly | QIODevice::Text)){
		QTextStream reportStream(&reportFile);
		for(int x = 0; x < allProcessedLines_.count(); x++){
			//qDebug() << "Report one line: " << allProcessedLines_.at(x);
			reportStream << allProcessedLines_.at(x);
		}
		reportFile.close();
	}

	progressBar_->hide();

	messageLabel_->setText("\nA report has been generated and sent to the Acquaman developers.\nThis window will close automatically, sorry for the inconveniece.");

	QTimer::singleShot(4500, this, SLOT(requestMonitorClose()));
	QTimer::singleShot(5000, this, SLOT(close()));
}

void AMCrashReporter::requestMonitorClose(){
	//qDebug() << "Going to send SIGUSR2 to " << monitorPID_;
	kill(monitorPID_, SIGUSR2);
}
