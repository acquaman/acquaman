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

	/// @brief Uses the recursive compare to look at the directories, but not do anything. No files are copied and no directories are created, but it gives a good state of affairs for checking before calling start(). Will be called automatically by start().
	AMRecursiveDirectoryCompare::DirectoryCompareResult prepare();

	/// @brief Starts the process of synchronizing the files in the two directories
	bool start();

	/// @brief Whether or not the process is currently copying files
	bool isRunning() const;

	/// @brief The file that's currently being copied. Empty if there is none.
	QString currentCopyFile() const;

	/// @brief The first directory
	QString side1Directory() const { return side1Directory_; }
	/// @brief The second directory
	QString side2Directory() const { return side2Directory_; }

	/// @brief The first directory's name
	QString side1DirectoryName() const { return side1DirectoryName_; }
	/// @brief The second directory's name
	QString side2DirectoryName() const { return side2DirectoryName_; }

	/// @brief Flag to say whether or not we're allowed to create side1 if it doesn't exist. False by default.
	bool allowSide1Creation() const { return allowSide1Creation_; }
	/// @brief Flag to say whether or not we're allowed to create side2 if it doesn't exist. False by default.
	bool allowSide2Creation() const { return allowSide2Creation_; }

	/// @brief Returns all of the error messages.
	QStringList errorMessages() const { return errorMessages_; }
	/// @brief Returns all of the progress messages.
	QStringList progressMessages() const { return progressMessages_; }

	/// @brief The object used to do the comparison between directories (will be null before compare() or start() is called)
	const AMRecursiveDirectoryCompare *comparator() { return comparator_; }

public slots:
	/// @brief Set the flag to say whether or not we're allowed to create side1 if it doesn't exist. False by default.
	void setAllowSide1Creation(bool allowSide1Creation);
	/// @brief Set the flag to say whether or not we're allowed to create side2 if it doesn't exist. False by default.
	void setAllowSide2Creation(bool allowSide2Creation);

	/// @brief Set the user meaningful name for the side 1 directory
	void setSide1DirectorName(const QString &side1DirectoryName);
	/// @brief Set the user meaningful name for the side 2 directory
	void setSide2DirectorName(const QString &side1DirectoryName);

	/// @brief Appends this exclusion pattern to the list
	void appendExcludePattern(const QString &excludePattern);

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
	/// @brief Signal emitted whenever we're copying a new file
	void currentCopyFileChanged(const QString &currentCopyFile);

	/// @brief Signal emitted whenever the progress values change. Relays all applicable information
	void progressChanged(int percentCompleteFile, int remainingFilesToCopy, int totalFilesToCopy);

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
	/// @brief Returns the state of the directory compare, whether side1 or side2 is newer, or whether it's in some other state.
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

protected:
	/// @brief The first directory
	QString side1Directory_;
	/// @brief The second directory
	QString side2Directory_;

	/// @brief The first directory's name
	QString side1DirectoryName_;
	/// @brief The second directory's name
	QString side2DirectoryName_;

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
	/// @brief Value that tells the progress of the copy for a single file
	int percentProgress_;
	/// @brief The number of files left for rsync to copy
	int remainingFilesToCopy_;
	/// @brief The total number of files for rsync to copy
	int totalFilesToCopy_;
	/// @brief Whether the process is currently running
	bool isRunning_;
	/// @brief The file that's currently being copied. Empty if there is none.
	QString currentCopyFile_;

	/// @brief The object used to do the comparison between directories
	AMRecursiveDirectoryCompare *comparator_;

	/// @brief The result of the last comparison that was done
	AMRecursiveDirectoryCompare::DirectoryCompareResult comparisonResult_;

	/// @brief This list contains patterns that we don't want to copy or compare between the two directories
	QStringList excludePatterns_;
};

#endif // AMDIRECTORYSYNCHRONIZER_H
