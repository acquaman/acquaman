#include "AMDirectorySynchronizer.h"
//#include "dataman/database/AMDatabase.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

AMDirectorySynchronizer::AMDirectorySynchronizer(const QString &side1Directory, const QString &side2Directory, QObject *parent)
	:QObject(parent)
{
	comparator_ = 0; //NULL
	comparisonResult_ = AMRecursiveDirectoryCompare::NotYetRun;

	side1Directory_ = side1Directory;
	side2Directory_ = side2Directory;

	side1DirectoryName_ = "Side1";
	side2DirectoryName_ = "Side2";

	allowSide1Creation_ = false;
	allowSide2Creation_ = false;

	copyProcess_ = new QProcess(this);
	isRunning_ = false;
	percentProgress_ = 0;

	connect(copyProcess_, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onCopyFinished(int, QProcess::ExitStatus)));
	connect(copyProcess_, SIGNAL(readyReadStandardError()), this, SLOT(onCopyReadyReadStdErr()));
	connect(copyProcess_, SIGNAL(readyReadStandardOutput()), this, SLOT(onCopyReadyReadStdOut()));
	connect(copyProcess_, SIGNAL(started()), this, SLOT(onCopyStarted()));
	appendToProgressMessage("Copy not yet started");
}

AMRecursiveDirectoryCompare::DirectoryCompareResult AMDirectorySynchronizer::prepare(){
	errorMessages_.clear();
	progressMessages_.clear();

	if(isRunning_){
		appendToErrorMessage(QString("Process is already running"));
		return AMRecursiveDirectoryCompare::InvalidResult;
	}

	comparisonResult_ = compareDirectories();

	if(comparisonResult_ == AMRecursiveDirectoryCompare::NeitherSideExistsResult)
		appendToErrorMessage(QString("Neither the %1 (%2) nor the %3 (%4) exist, cannot proceed.").arg(side1DirectoryName_).arg(side1Directory_).arg(side2DirectoryName_).arg(side2Directory_));
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::Side1DoesNotExistResult){
		if(!allowSide1Creation())
			appendToErrorMessage(QString("Side %1 (%2) does not exist and creation is not allowed, cannot proceed.").arg(side1DirectoryName_).arg(side1Directory_));
		else
			appendToProgressMessage(QString("Side %1 (%2) does not exist yet. Depending on creation we may be able to proceed.").arg(side1DirectoryName_).arg(side1Directory_));
	}
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::Side2DoesNotExistResult){
		if(!allowSide2Creation())
			appendToErrorMessage(QString("Side %1 (%2) does not exist and creation is not allowed, cannot proceed.").arg(side2DirectoryName_).arg(side2Directory_));
		else
			appendToProgressMessage(QString("Side %1 (%2) does not exist yet. Depending on creation we may be able to proceed.").arg(side2DirectoryName_).arg(side2Directory_));
	}
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::BothSidesAreFiles)
		appendToErrorMessage(QString("Both the %1 (%2) and the %3 (%4) exist but are files, cannot proceed.").arg(side1DirectoryName_).arg(side1Directory_).arg(side2DirectoryName_).arg(side2Directory_));
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::Side1IsFile)
		appendToErrorMessage(QString("Side %1 (%2) exists but is a file, cannot proceed.").arg(side1DirectoryName_).arg(side1Directory_));
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::Side2IsFile)
		appendToErrorMessage(QString("Side %1 (%2) exists but is a file, cannot proceed.").arg(side2DirectoryName_).arg(side2Directory_));
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::UnableToDetermineResult)
		appendToErrorMessage(QString("Unable to determine which directory contains the newest version."));
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::InvalidResult)
		appendToErrorMessage(QString("Unknown error has occured in AMDirectorySynchronizer"));
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::BothSidesModifiedResult)
		appendToErrorMessage(QString("Contents of %1 and %2 have both changed, cannot proceed").arg(side1Directory_).arg(side2Directory_));
	else if(comparisonResult_ == AMRecursiveDirectoryCompare::FullyMatchingResult)
		appendToProgressMessage(QString("Contents of %1 and %2 are the same, no copying necessary.").arg(side1Directory_).arg(side2Directory_));

	emit prepared(comparisonResult_);
	return comparisonResult_;
}

bool AMDirectorySynchronizer::start()
{
	if(comparisonResult_ == AMRecursiveDirectoryCompare::NotYetRun)
		comparisonResult_ = prepare();

	switch(comparisonResult_){
	case AMRecursiveDirectoryCompare::Side1DoesNotExistResult:
		if(allowSide1Creation()){
			QDir().mkpath(side1Directory_);
			if(!QFile::exists(side1Directory_)){
				appendToErrorMessage(QString("Side %1 (%2) does not exist and an attempt at creation failed, cannot proceed.").arg(side1DirectoryName_).arg(side1Directory_));
				return false;
			}
		}
		else
			return false;
		break;
	case AMRecursiveDirectoryCompare::Side2DoesNotExistResult:
		if(allowSide2Creation()){
			QDir().mkpath(side2Directory_);
			if(!QFile::exists(side2Directory_)){
				appendToErrorMessage(QString("Side %1 (%2) does not exist and an attempt at creation failed, cannot proceed.").arg(side2DirectoryName_).arg(side2Directory_));
				return false;
			}
		}
		else
			return false;
		break;
	case AMRecursiveDirectoryCompare::FullyMatchingResult:
		percentProgress_ = 100;
		emit percentageProgressChanged(100);
		QTimer::singleShot(1000, this, SIGNAL(copyCompleted()));
		return true;
		break;
	case AMRecursiveDirectoryCompare::Side1ModifiedResult:
		// Taken care of below
		break;
	case AMRecursiveDirectoryCompare::Side2ModifiedResult:
		// Taken care of below
		break;
	case AMRecursiveDirectoryCompare::NeitherSideExistsResult:
	case AMRecursiveDirectoryCompare::BothSidesAreFiles:
	case AMRecursiveDirectoryCompare::Side1IsFile:
	case AMRecursiveDirectoryCompare::Side2IsFile:
	case AMRecursiveDirectoryCompare::UnableToDetermineResult:
	case AMRecursiveDirectoryCompare::InvalidResult:
	case AMRecursiveDirectoryCompare::BothSidesModifiedResult:
	default:
		return false;
	}

	QString process = "rsync";
	QStringList args;
	args << "-avt" << "--progress";
	for(int x = 0, size = excludePatterns_.size(); x < size; x++)
		args << "--exclude" << excludePatterns_.at(x);

	if(comparisonResult_ == AMRecursiveDirectoryCompare::Side1ModifiedResult){
		lockDirectory(side1Directory_);
		args << QString("%1/.").arg(side1Directory_) << QString("%1/").arg(side2Directory_);
	}
	else{
		lockDirectory(side2Directory_);
		args << QString("%1/.").arg(side2Directory_) << QString("%1/").arg(side1Directory_);

	}

	copyProcess_->start(process, args);
	if(copyProcess_->waitForStarted(30000)){
		isRunning_ = true;
		return true;
	}
	else
		appendToErrorMessage("Could not start the sync process");
	return false;
}

bool AMDirectorySynchronizer::isRunning() const{
	return isRunning_;
}

QString AMDirectorySynchronizer::currentCopyFile() const{
	return currentCopyFile_;
}

void AMDirectorySynchronizer::setAllowSide1Creation(bool allowSide1Creation){
	allowSide1Creation_ = allowSide1Creation;
}

void AMDirectorySynchronizer::setAllowSide2Creation(bool allowSide2Creation){
	allowSide2Creation_ = allowSide2Creation;
}

void AMDirectorySynchronizer::setSide1DirectorName(const QString &side1DirectoryName){
	side1DirectoryName_ = side1DirectoryName;
}

void AMDirectorySynchronizer::setSide2DirectorName(const QString &side2DirectoryName){
	side2DirectoryName_ = side2DirectoryName;
}

void AMDirectorySynchronizer::appendExcludePattern(const QString &excludePattern){
	excludePatterns_.append(excludePattern);
}

AMRecursiveDirectoryCompare::DirectoryCompareResult AMDirectorySynchronizer::compareDirectories()
{
	if(comparator_){
		delete comparator_;
		comparator_ = 0; //NULL
	}
	comparator_ = new AMRecursiveDirectoryCompare(side1Directory_, side2Directory_, excludePatterns_, side1DirectoryName_, side2DirectoryName_);

	return comparator_->compare();
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
	QRegExp matchesDatabaseFile("*.db", Qt::CaseSensitive, QRegExp::Wildcard);
	QRegExp matchesFilePath("*/*.*", Qt::CaseSensitive, QRegExp::Wildcard);

	QString simplifiedInput = input.simplified();

	QStringList separatedInput = simplifiedInput.split(" ");

	if(separatedInput.count() > 1)
	{
		QString valueString = separatedInput.at(1);
		valueString.replace("%", "", Qt::CaseInsensitive);
		bool successfulParse = false;

		int valueConvert = valueString.toInt(&successfulParse);

		if(successfulParse && valueConvert <= 100)
		{
			for(int x = 0, size = separatedInput.count(); x < size; x++){
				if(separatedInput.at(x).contains("to-check=")){
					bool parseRemaining = false;
					bool parseTotal = false;
					int remaining = separatedInput.at(x).section('=', 1, 1).remove(')').section('/', 0, 0).toInt(&parseRemaining);
					int total = separatedInput.at(x).section('=', 1, 1).remove(')').section('/', 1, 1).toInt(&parseTotal);
					if(parseRemaining && parseTotal){
						remainingFilesToCopy_ = remaining;
						totalFilesToCopy_ = total;
					}
				}
				else if(matchesDatabaseFile.indexIn(separatedInput.at(x)) != -1 || matchesFilePath.indexIn(separatedInput.at(x)) != -1){
					if(currentCopyFile_ != separatedInput.at(x)){
						currentCopyFile_ = separatedInput.at(x);
						emit currentCopyFileChanged(currentCopyFile_);
					}
				}
			}

			percentProgress_ = valueConvert;
			emit percentageProgressChanged(percentProgress_);
			emit progressChanged(percentProgress_, remainingFilesToCopy_, totalFilesToCopy_);
		}
	}
	else if(separatedInput.count() == 1){
		if(matchesDatabaseFile.indexIn(separatedInput.at(0)) != -1 || matchesFilePath.indexIn(separatedInput.at(0)) != -1){
			if(currentCopyFile_ != separatedInput.at(0)){
				currentCopyFile_ = separatedInput.at(0);
				emit currentCopyFileChanged(currentCopyFile_);
			}
		}
	}
}

void AMDirectorySynchronizer::lockDirectory(const QString &path)
{
	QDir directory(path);

	directory.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

	// Get a list of all direct subdirectories of this directory, and files it contains
	QFileInfoList directoryEntries = directory.entryInfoList();

	for (int iDirectoryEntry = 0; iDirectoryEntry < directoryEntries.count(); iDirectoryEntry++)
	{
		QFileInfo currentDirectoryEntry = directoryEntries.at(iDirectoryEntry);

		if(currentDirectoryEntry.isFile()) {
			QFile currentFile(currentDirectoryEntry.absoluteFilePath());
			currentFile.setPermissions(QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther);
		}
		else if(currentDirectoryEntry.isDir()) {
			lockDirectory(currentDirectoryEntry.absoluteFilePath());
		}

	}

}

void AMDirectorySynchronizer::unlockDirectory(const QString &path)
{
	QDir directory(path);

	directory.setFilter(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);

	// Get a list of all direct subdirectories of this directory, and files it contains
	QFileInfoList directoryEntries = directory.entryInfoList();

	for (int iDirectoryEntry = 0; iDirectoryEntry < directoryEntries.count(); iDirectoryEntry++)
	{
		QFileInfo currentDirectoryEntry = directoryEntries.at(iDirectoryEntry);

		if(currentDirectoryEntry.isFile()) {
			QFile currentFile(currentDirectoryEntry.absoluteFilePath());
			currentFile.setPermissions(QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther | QFile::WriteOwner );
		}
		else if(currentDirectoryEntry.isDir()) {
			unlockDirectory(currentDirectoryEntry.absoluteFilePath());
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
	unlockDirectory(side2Directory_);
	unlockDirectory(side1Directory_);
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
