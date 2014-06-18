#include "AMDirectorySynchronizer.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
AMDirectorySynchronizer::AMDirectorySynchronizer(const QString &sourceDirectory, const QString &destinationDirectory, QObject *parent)
{
	sourceDirectory_ = sourceDirectory;
	destinationDirectory_ = destinationDirectory;

	connect(&copyProcess_, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onCopyFinished(int, QProcess::ExitStatusi)));
	connect(&copyProcess_, SIGNAL(readyReadStandardError()), this, SLOT(onCopyReadyReadStdErr()));
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
			setLastMessage(QString("Error: Path %1 does not exist, and could not be created").arg(path));
			return false;
		}

		return true;
	}

	// 'File' exists, but is it a directory, or a file
	QFileInfo pathInfo(path);
	if(pathInfo.isFile())
	{
		setLastMessage(QString("Error: Path %1 exists, but refers to a file, not a directory").arg(path));
		return false;
	}

	return true;

}

bool AMDirectorySynchronizer::start()
{
	if(!(validatePath(sourceDirectory_) && validatePath(destinationDirectory_)))
		return false;

	AMRecursiveDirectoryCompare::DirectoryCompareResult result = compareDirectories();

	if(result == AMRecursiveDirectoryCompare::FullyMatchingResult)
	{
		setLastMessage(QString("Contents of %1 and %2 are the same, no copying necessary.").arg(sourceDirectory_).arg(destinationDirectory_));
		return false;
	}

	if(result == AMRecursiveDirectoryCompare::InvalidResult)
	{
		setLastMessage(QString("Unknown error has occured in AMDirectorySynchronizer"));
		return false;
	}

	if(result == AMRecursiveDirectoryCompare::BothSidesModifiedResult)
	{
		setLastMessage(QString("Contents of %1 and %2 have both changed, cannot proceed").arg(sourceDirectory_).arg(destinationDirectory_));
	}



	QString process = "cp";
	QStringList args;
	args << "-u"
		 << "-r";

	if(result == AMRecursiveDirectoryCompare::Side1ModifiedResult)
	{
		args << sourcePath_
			 << destinationPath_;
	}
	else
	{
		args << destinationPath_
			 << sourcePath_;
	}
	copyProcess_.start(process, args);
	return true;
}


void AMDirectorySynchronizer::setLastMessage(const QString &message)
{
	lastMessage_ = message;
	emit lastMessageChanged(lastMessage_);
}


AMRecursiveDirectoryCompare::DirectoryCompareResult AMDirectorySynchronizer::compareDirectories()
{
	AMRecursiveDirectoryCompare compare(source, destination);

	return compare.compare();
}


void AMDirectorySynchronizer::onCopyReadyReadStdErr()
{
	setLastMessage(copyProcess_.readAllStandardError().data());
}

void AMDirectorySynchronizer::onCopyFinished(int exitCode, QProcess::ExitStatus status)
{
	if(exitCode == 1 || status == QProcess::CrashExit)
	{
		emit copyError();
	}
	else
	{
		emit copyCompleted();
	}
}
