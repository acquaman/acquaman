#include "AMDirectorySynchronizer.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
AMDirectorySynchronizer::AMDirectorySynchronizer(const QString &sourceDirectory, const QString &destinationDirectory, QObject *parent)
	:QObject(parent)
{
	sourceDirectory_ = sourceDirectory;
	destinationDirectory_ = destinationDirectory;
	copyProcess_ = new QProcess(this);
	isRunning_ = false;
	percentProgress_ = 0;
	timer_ = new QTimer(this);
	timer_->setInterval(1000);

	connect(copyProcess_, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onCopyFinished(int, QProcess::ExitStatus)));
	connect(copyProcess_, SIGNAL(readyReadStandardError()), this, SLOT(onCopyReadyReadStdErr()));
	connect(copyProcess_, SIGNAL(readyReadStandardOutput()), this, SLOT(onCopyReadyReadStdOut()));
	connect(copyProcess_, SIGNAL(started()), this, SLOT(onCopyStarted()));
	connect(timer_, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
	appendToProgressMessage("Copy not yet started");
}

bool AMDirectorySynchronizer::validatePath(const QString &path)
{

	if(!QFile::exists(path))
	{
		// Doesn't exit, create:
		QDir().mkdir(path);

		// Check if has been created properly
		if(!QFile::exists(path))
		{
			appendToErrorMessage(QString("Error: Path %1 does not exist, and could not be created").arg(path));
			return false;
		}

		return true;
	}

	// 'File' exists, but is it a directory, or a file
	QFileInfo pathInfo(path);
	if(pathInfo.isFile())
	{
		appendToErrorMessage(QString("Error: Path %1 exists, but refers to a file, not a directory").arg(path));
		return false;
	}

	return true;

}

bool AMDirectorySynchronizer::start()
{
	errorMessages_.clear();
	progressMessages_.clear();

	if(isRunning_)
	{
		appendToErrorMessage(QString("Process is already running"));
		return false;
	}

	if(!(validatePath(sourceDirectory_) && validatePath(destinationDirectory_)))
		return false;

	AMRecursiveDirectoryCompare::DirectoryCompareResult result = compareDirectories();

	if(result == AMRecursiveDirectoryCompare::FullyMatchingResult)
	{
		appendToProgressMessage(QString("Contents of %1 and %2 are the same, no copying necessary.").arg(sourceDirectory_).arg(destinationDirectory_));
		timer_->start();
		return true;
	}

	if(result == AMRecursiveDirectoryCompare::InvalidResult)
	{
		appendToErrorMessage(QString("Unknown error has occured in AMDirectorySynchronizer"));
		return false;
	}

	if(result == AMRecursiveDirectoryCompare::BothSidesModifiedResult)
	{
		appendToErrorMessage(QString("Contents of %1 and %2 have both changed, cannot proceed").arg(sourceDirectory_).arg(destinationDirectory_));
		return false;
	}

	QString process = "rsync";
	QStringList args;
	args << "-avt"
		 << "--progress";

	if(result == AMRecursiveDirectoryCompare::Side1ModifiedResult)
	{
		args << QString("%1/.").arg(sourceDirectory_)
			 << QString("%1/").arg(destinationDirectory_);
	}
	else
	{
		args << QString("%1/.").arg(destinationDirectory_)
			 << QString("%1/").arg(sourceDirectory_);
	}
	copyProcess_->start(process, args);
	if(copyProcess_->waitForStarted(30000))
	{
		isRunning_ = true;
		return true;
	}
	else
		appendToErrorMessage("Could not start the sync process");
	return false;
}

bool AMDirectorySynchronizer::isRunning()
{
	return isRunning_;
}

AMRecursiveDirectoryCompare::DirectoryCompareResult AMDirectorySynchronizer::compareDirectories()
{
	AMRecursiveDirectoryCompare compare(sourceDirectory_, destinationDirectory_);

	return compare.compare();
}

void AMDirectorySynchronizer::appendToErrorMessage(const QString &message)
{
	errorMessages_ << message;
	emit errorMessagesChanged(message);
}

void AMDirectorySynchronizer::appendToProgressMessage(const QString &message)
{
	progressMessages_ << message;
	emit progressMessagesChanged(message);
}

void AMDirectorySynchronizer::parseProgressInput(const QString &input)
{

	QString simplifiedInput = input.simplified();

	QStringList separatedInput = simplifiedInput.split(" ");

	if(separatedInput.count() > 1)
	{
		QString valueString = separatedInput.at(1);
		valueString.replace("%", "", Qt::CaseInsensitive);
		qDebug() << valueString;
		bool successfulParse = false;

		int valueConvert = valueString.toInt(&successfulParse);

		if(successfulParse)
		{
			percentProgress_ = valueConvert;
			emit percentageProgressChanged(percentProgress_);
		}
	}
}


void AMDirectorySynchronizer::onCopyReadyReadStdErr()
{
	appendToErrorMessage(copyProcess_->readAllStandardError().data());
}

void AMDirectorySynchronizer::onCopyReadyReadStdOut()
{
	QString message = copyProcess_->readAllStandardOutput().data();
	parseProgressInput(message);
	appendToProgressMessage(message);
}

void AMDirectorySynchronizer::onCopyFinished(int exitCode, QProcess::ExitStatus status)
{
	isRunning_ = false;
	if(exitCode == 1 || status == QProcess::CrashExit)
	{
		emit copyFailed();
	}
	else
	{
		emit copyCompleted();
	}
}

void AMDirectorySynchronizer::onCopyStarted()
{
	appendToProgressMessage("Started");
}

void AMDirectorySynchronizer::onTimerTimeout()
{
	emit copyCompleted();
}
