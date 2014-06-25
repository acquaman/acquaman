#ifndef AMDIRECTORYSYNCHRONIZER_H
#define AMDIRECTORYSYNCHRONIZER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include "AMRecursiveDirectoryCompare.h"
/**
 * @brief Class designed to recursively synchronize the data in two folders, while
 * checking the validity of the states of both directories before proceeding
 */
class AMDirectorySynchronizer : public QObject
{
	Q_OBJECT
private:
	/// @brief The directory to copy to
	QString destinationDirectory_;
	/// @brief The directory to copy from
	QString sourceDirectory_;
	/// @brief A string list containing all the standard output messages emitted by the QProcess
	QStringList progressMessages_;
	/// @brief A string list of error messages emitted by the QProcess
	QStringList errorMessages_;
	/// @brief A process which actually performs the copy
	QProcess* copyProcess_;
	/// @brief Value that tells the progress of the copy
	int percentProgress_;
	/// @brief Whether the process is currently running
	bool isRunning_;
	/// @brief Timer used when no copying is necessary to wait a few seconds before emitting the copyComplete signal
	QTimer* timer_;
public:
	/**
	 * @brief Creates an instance of an AMDirectorySynchronizer with the provided sourceDirectory
	 * and destinationDirectory
	 * @param sourceDirectory::QString ~ The path of the directory to copy data from
	 * @param destinationDirectory::QString ~ The path of the directory to copy data to
	 */
	explicit AMDirectorySynchronizer(const QString& sourceDirectory, const QString& destinationDirectory, QObject *parent = 0);
	/// @brief Simple validation of path. Ensures it exists, and is a folder, not a file
	bool validatePath(const QString& path);
	/// @brief Starts the process of copying the files from the source to the destination
	bool start();
	/// @brief Whether or not the process is currently copying files
	bool isRunning();
	/// @brief The source directory from which the copy is performed
	QString sourceDirectory() const { return sourceDirectory_; }
	/// @brief The destination directory to which the data is copied
	QString destinationDirectory() const { return destinationDirectory_; }

protected:
	/// @brief Checks to see if any files exist in the source but not the directory, or
	/// vice versa, or both
	AMRecursiveDirectoryCompare::DirectoryCompareResult compareDirectories();

	/// @brief Appends to the error message list
	void appendToErrorMessage(const QString& message);

	/// @brief Appends to progress message list
	void appendToProgressMessage(const QString& message);

	/// @brief Parses input, checks if it of the rsync --progress format, strips and
	/// coverts the percentage into a value, sets the percentProgress to this and emits percentageProgressChanged
	void parseProgressInput(const QString& input);

	/// @brief Recursively makes the contents of the given path read only
	void lockDirectory(const QString& path);
	/// @brief Recusively makes the contents of the given path read/write
	void unlockDirectory(const QString &path);
signals:
	/// @brief Signal emitted when the copy is completed successfully
	void copyCompleted();
	/// @brief Signal emitted when the copy terminates unexpectedly after starting
	void copyFailed();
	/// @brief Signal emitted whenever the Progress Messages changes
	void progressMessagesChanged(const QString& value);
	/// @brief Signal emitted whenever the Error Messages changes
	void errorMessagesChanged(const QString& value);
	/// @brief Signal emitted whenever the percentageProgress value changes
	void percentageProgressChanged(int);
protected slots:
	/// @brief Handles the process having Standard Error output to read (appends it to errorMessages_)
	void onCopyReadyReadStdErr();
	/// @brief Handles the process having Standard Output to read (appends it to progressMessages_)
	void onCopyReadyReadStdOut();
	/// @brief Handles the copy process exiting
	void onCopyFinished(int, QProcess::ExitStatus);
	/// @brief Handles to copy process starting
	void onCopyStarted();
	/// @brief Handles the timer timeout signal. Emits copyComplete
	void onTimerTimeout();
public slots:
	
};

#endif // AMDIRECTORYSYNCHRONIZER_H
