#ifndef AMDIRECTORYSYNCHRONIZERDIALOG_H
#define AMDIRECTORYSYNCHRONIZERDIALOG_H

#include <QDialog>

#include "util/AMDirectorySynchronizer.h"

class QTextEdit;
class QPushButton;
class QProgressBar;
class QLabel;

class AMVerticalStackWidget;

class AMDirectorySynchronizerDialog : public QDialog
{
	Q_OBJECT

public:
	/// Dialog to help synchronize two directories
	explicit AMDirectorySynchronizerDialog(const QString& side1Directory, const QString& side2Directory, const QString& side1DirectoryName = "Side1", const QString& side2DirectoryName = "Side2",QWidget *parent = 0);
	~AMDirectorySynchronizerDialog();

	/// Call prepare to check on the current state. Maybe no synchronization is necessary, maybe one way synchronization is not possible, an explanation will wind up in error, progress, and/or file listings boxes.
	void prepare();
	/// Call start to synchronize the directories. Prepare will be called first.
	bool start();

	/// Returns the path of the first directory
	QString side1Directory() const;
	/// Returns the path of the second directory
	QString side2Directory() const;

	/// Returns the conceptual name of the first directory (maybe local)
	QString side1DirectoryName() const;
	/// Returns the conceptual name of the second directory (maybe network)
	QString side2DirectoryName() const;

public slots:
	/// With this option on, the dialog will call quit() on the QApplication when it's done
	void setCloseOnCompletion(bool closeOnCompletion);
	/// With this option on, the dialog will show a countdown window on completion (letting the user either close the dialog or simply dissapearing on its own)
	void setTimedWarningOnCompletion(bool timedWarningOnCompletion);

	/// Adds another pattern that the synchronizer will ignore (any file that end in ".db" would be "*.db")
	void appendExcludePattern(const QString &excludePattern);

	/// Calls prepare() and exec()
	void autoPrepare();
	/// Calls start() and exec()
	bool autoStart();

	/// Calls start() after \param delaySeconds
	void delayedStart(double delaySeconds);

signals:
	/// Signals when the dialog has completed
	void dialogCompleted();

	/// Signal emitted when the we have finished processing the output of the synchronizer prepared() signal
	void prepared();

protected slots:
	/// Handles changes to the error text from the underlying synchronizer
	void onSynchronizerErrorTextChanged(const QString& message);
	/// Handles updates about the progress of the synchronizer
	void onSynchronizerProgressTextChanged(const QString& message);
	/// Handles the completion message from the underlying synchronizer
	void onSynchronizerComplete();
	/// Handles the failed message from the underlying synchronizer
	void onSynchronizerFailed();

	/// Handles update from the underlying synchronizer about the rsync progress
	void onProgressChanged(int percentCompleteFile, int remainingFilesToCopy, int totalFilesToCopy);
	/// Handles some status label changes based on changing what file is currently being copied
	void onCurrentCopyFileChanged(const QString &currentCopyFile);

	/// The button appears when we can close safely and handles the close
	void onCloseButtonClicked();

	/// Calls prepare() when the prepare button is clicked
	void onPrepareButtonClicked();
	/// Calls start() when the start button is clicked
	void onStartButtonClicked();

	/// Handles decrementing the delay countdown when delayedStart() is called with a non-zero time
	void onDelayCountdown();

	/// Sometime we get into a race condition with regard to prepare, this helps us wait until we can successfully proceed
	void onPreparedHelper();
	/// Handles the prepared() signal from the synchronizer and emits our prepared() signal at the end
	void onPrepared(AMRecursiveDirectoryCompare::DirectoryCompareResult comparisonResult);
	/// Handles calling the rest of the start routine after start first calls prepare()
	void onStartPrepared();

protected:
	/// Overrides the close event, so that the user doesn't close the dialog during copying
	void closeEvent(QCloseEvent *);

	/// Quickly returns the unique files, directories, and newer files from side 1
	QString side1ModifiedResultsString() const;
	/// Quickly returns the unique files, directories, and newer files from side 2
	QString side2ModifiedResultsString() const;

protected:
	/// Path to the side 1 directory
	QString side1Directory_;
	/// Path to the side 2 directory
	QString side2Directory_;

	/// Holds the human-readable name for side 1
	QString side1DirectoryName_;
	/// Holds the human-readable name for side 2
	QString side2DirectoryName_;

	/// The main status label for the whole dialog, updated to tell the user about the current situation
	QLabel *mainStatusLabel_;

	/// Progress bar for the file we're currently copying
	QProgressBar *singleFileProgressBar_;
	/// Label for the current file transfer percentage
	QLabel *singleFileLabel_;
	/// Progress bar for the entire transfer
	QProgressBar *overallTransferProgressBar_;
	/// Label for the overall transfer percentage
	QLabel *overallTransferLabel_;
	/// Label to hold the name of the file we're currently copying
	QLabel *currentCopyFileLabel_;

	/// Text edit holding any error messages
	QTextEdit *errorTextEdit_;
	/// Close button that appears if errors occured
	QPushButton *errorCloseButton_;
	/// Text edit holding any progress messages
	QTextEdit *progressTextEdit_;
	/// Text edit holding the listing of files for synchronization
	QTextEdit *fileListingEdit_;
	/// Vertical stack widget that contains the three text edits so they can be minimized and hidden if necessary
	AMVerticalStackWidget *feedbackStackWidget_;

	/// The underlying synchronizer for the dialog
	AMDirectorySynchronizer *synchronizer_;
	/// The result of the last prepare() comparison
	AMRecursiveDirectoryCompare::DirectoryCompareResult lastCompareResult_;


	/// Button to call prepare
	QPushButton *prepareButton_;
	/// Button to call start
	QPushButton *startButton_;

	/// Flag to latch a successful synchronization
	bool successfulSync_;
	/// Flag to enable the quit() call on completion
	bool closeOnCompletion_;
	/// Flag to enable the timeout message box on completion
	bool timedWarningOnCompletion_;

	/// The number of seconds we will delay before starting if delayedStart() is called
	double delaySeconds_;
	/// A counter to help with the delayedStart status timer
	int delayCountdown_;
	/// Timer to update the label for counting down to start when delayedStart() is called
	QTimer *delayCountdownTimer_;
};

#endif // AMDIRECTORYSYNCHRONIZERDIALOG_H
