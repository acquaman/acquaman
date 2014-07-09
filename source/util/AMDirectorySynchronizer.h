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

public:
	/**
	 * @brief Creates an instance of an AMDirectorySynchronizer with the provided directories
	 * @param side1Directory::QString ~ The path of the first directory
	 * @param side2Directory::QString ~ The path of the second directory
	 */
	explicit AMDirectorySynchronizer(const QString& side1Directory, const QString& side2Directory, QObject *parent = 0);

	/// @brief Starts the process of synchronizing the files in the two directories
	bool start();
	/// @brief Returns the state of the directory compare, whether side1 or side2 is newer, or whether it's in some other state.
	AMRecursiveDirectoryCompare::DirectoryCompareResult compareDirectories();
	/// @brief Whether or not the process is currently copying files
	bool isRunning();

	/// @brief The first directory
	QString side1Directory() const { return side1Directory_; }
	/// @brief The second directory
	QString side2Directory() const { return side2Directory_; }

	/// @brief Flag to say whether or not we're allowed to create side1 if it doesn't exist. False by default.
	bool allowSide1Creation() const { return allowSide1Creation_; }
	/// @brief Flag to say whether or not we're allowed to create side2 if it doesn't exist. False by default.
	bool allowSide2Creation() const { return allowSide2Creation_; }

public slots:
	/// @brief Set the flag to say whether or not we're allowed to create side1 if it doesn't exist. False by default.
	void setAllowSide1Creation(bool allowSide1Creation);
	/// @brief Set the flag to say whether or not we're allowed to create side2 if it doesn't exist. False by default.
	void setAllowSide2Creation(bool allowSide2Creation);

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

protected:
	/// @brief Simple validation of path. Ensures it exists, and is a folder, not a file. If it doesn't exist, attempts to create if allowCreation is true.
	bool validatePath(const QString &path, bool allowCreation);

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

private:
	/// @brief The first directory
	QString side1Directory_;
	/// @brief The second directory
	QString side2Directory_;

	/// @brief Flag to say whether or not we're allowed to create side1 if it doesn't exist. False by default.
	bool allowSide1Creation_;
	/// @brief Flag to say whether or not we're allowed to create side2 if it doesn't exist. False by default.
	bool allowSide2Creation_;

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
	
};

#endif // AMDIRECTORYSYNCHRONIZER_H
