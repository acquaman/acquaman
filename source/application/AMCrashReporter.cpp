#include "AMCrashReporter.h"

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
#include <QCloseEvent>
#include <QMessageBox>

int AMCrashReporterUnixSignalHandler::sigusr1Fd[2];
int AMCrashReporterUnixSignalHandler::sigusr2Fd[2];

 AMCrashReporterUnixSignalHandler::~AMCrashReporterUnixSignalHandler(){}
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
	Q_UNUSED(signum)
	char a = 1;
	ssize_t retVal = ::write(sigusr1Fd[0], &a, sizeof(a));
	Q_UNUSED(retVal);
}

void AMCrashReporterUnixSignalHandler::sigusr2SignalHandler(int signum){
	Q_UNUSED(signum)
	char a = 1;
	ssize_t retVal = ::write(sigusr2Fd[0], &a, sizeof(a));
	Q_UNUSED(retVal);
}

 AMCrashReporterStackTraceSymbol::~AMCrashReporterStackTraceSymbol(){}
AMCrashReporterStackTraceSymbol::AMCrashReporterStackTraceSymbol(const QString &executable, const QString &address, QObject *parent) :
	QObject(parent)
{
	executable_ = executable;
	address_ = address;
	invalid_ = false;
}

 AMCrashMonitor::~AMCrashMonitor(){}
AMCrashMonitor::AMCrashMonitor(const QString &executableFullPath, const QString &errorFilePath, int watchingPID, QObject *parent) :
	QObject(parent)
{
	executableFullPath_ = executableFullPath;
	errorFilePath_ = errorFilePath;
	watchingPID_ = watchingPID;

	unixSignalHandler_ = new AMCrashReporterUnixSignalHandler();
	connect(unixSignalHandler_, SIGNAL(sigusr1Detected()), this, SLOT(onSiguser1Detected()));
	connect(unixSignalHandler_, SIGNAL(sigusr2Detected()), this, SLOT(onSiguser2Detected()));

	watchingPIDTimer_ = new QTimer(this);
	watchingPIDTimer_->setInterval(5000);
	connect(watchingPIDTimer_, SIGNAL(timeout()), this, SLOT(onWatchingPIDTimerTimeout()));

	crashReporterPIDTimer_ = new QTimer(this);
	crashReporterPIDTimer_->setInterval(5000);
	connect(crashReporterPIDTimer_, SIGNAL(timeout()), this, SLOT(onCrashReporterPIDTimerTimeout()));

	watchingPIDTimer_->start();
}

void AMCrashMonitor::onSiguser1Detected(){
	watchingPIDTimer_->stop();

	QStringList arguments;
	arguments << executableFullPath_;
	arguments << errorFilePath_;
	arguments << QString("%1").arg(watchingPID_);
	arguments << QString("%1").arg(getpid());
	QProcess::startDetached(QCoreApplication::instance()->arguments().at(0), arguments, QDir::currentPath(), &crashReporterPID_);

	crashReporterPIDTimer_->start();
}

void AMCrashMonitor::onSiguser2Detected(){
	//qDebug() << "Detected SIGUSR2 in AMCrashMonitor, need to close this program now";
	crashReporterPIDTimer_->stop();

	QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
}

void AMCrashMonitor::onWatchingPIDTimerTimeout(){
	AMProcessWatcher *checkForWatchingPID = new AMProcessWatcher(watchingPID_);
	connect(checkForWatchingPID, SIGNAL(foundWatchPID(bool)), this, SLOT(onWatchingPIDFoundProcess(bool)));
	checkForWatchingPID->startWatching();
}

void AMCrashMonitor::onCrashReporterPIDTimerTimeout(){
	AMProcessWatcher *checkForWatchingPID = new AMProcessWatcher(crashReporterPID_);
	connect(checkForWatchingPID, SIGNAL(foundWatchPID(bool)), this, SLOT(onCrashReporterPIDFoundProcess(bool)));
	checkForWatchingPID->startWatching();
}

void AMCrashMonitor::onWatchingPIDFoundProcess(bool wasFound){
	if(!wasFound)
		QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
}

void AMCrashMonitor::onCrashReporterPIDFoundProcess(bool wasFound){
	if(!wasFound)
		QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
}

AMCrashReporter::AMCrashReporter(const QString &executableFullPath, const QString &errorFilePath, int watchingPID, int monitorPID, QWidget *parent) :
	QWidget(parent)
{
	executableFullPath_ = executableFullPath;
	errorFilePath_ = errorFilePath;
	watchingPID_ = watchingPID;
	monitorPID_ = monitorPID;
	activeAddressConversion_ = 0;

	unixSignalHandler_ = new AMCrashReporterUnixSignalHandler();
	connect(unixSignalHandler_, SIGNAL(sigusr1Detected()), this, SLOT(onSiguser1Detected()));


	titleLabel_ = new QLabel("This is embarassing, it looks like \nAcquaman has crashed");
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

	//qDebug() << "Does the error file exist?" << QFile::exists(QString("/tmp/ErrorFile%1.txt").arg(watchingPID_));

	QString errorString;
	QString executableString;
	QString addressString;
	AMCrashReporterStackTraceSymbol *oneSymbol;
	QFile errorFile(QString("/tmp/ErrorFile%1.txt").arg(watchingPID_));
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
	QFile reportFile(QString("%1/report_%2_%3_%4_%5.txt").arg(errorFilePath_).arg(executableFullPath_.section('/', -1)).arg(QHostInfo::localHostName()).arg(QDateTime::currentDateTime().toString("hhmmss_ddMMyyyy")).arg(watchingPID_));
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

void AMCrashReporter::closeEvent(QCloseEvent *e){
	QMessageBox msgBox;
	msgBox.setText("Please Don't Close This Window.");
	msgBox.setInformativeText("Please don't close this window unless it's really necessary. It should automatically close shortly.");
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Ignore);
	msgBox.setDefaultButton(QMessageBox::Ok);
	int ret = msgBox.exec();

	switch(ret){
	case QMessageBox::Ok :
		e->ignore();
		break;
	case QMessageBox::Ignore :
		QWidget::closeEvent(e);
		break;
	default:
		e->ignore();
	}
}

AMProcessWatcher::AMProcessWatcher(qint64 watchPID, QObject *parent) :
	QObject(parent)
{
	watchPID_ = watchPID;
	foundProcess_ = false;
}

void AMProcessWatcher::startWatching(){
	psCommand_ = new QProcess(this);

	QString programName = "ps";
	QStringList programArguments;
	programArguments << "ax";
	programArguments << "-o";
	programArguments << "pid";

	connect(psCommand_, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessReadReady()));
	connect(psCommand_, SIGNAL(finished(int)), this, SLOT(onProcessFinished()));

	psCommand_->start(programName, programArguments);
}

void AMProcessWatcher::onProcessReadReady(){
	QString someOutput = psCommand_->readAllStandardOutput();
	if(!foundProcess_ && someOutput.contains(QString("%1").arg(watchPID_)))
		foundProcess_ = true;
}

void AMProcessWatcher::onProcessFinished(){
	emit foundWatchPID(foundProcess_);
}
