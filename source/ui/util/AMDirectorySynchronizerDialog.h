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
	explicit AMDirectorySynchronizerDialog(const QString& side1Directory, const QString& side2Directory, const QString& side1DirectoryName = "Side1", const QString& side2DirectoryName = "Side2",QWidget *parent = 0);
	~AMDirectorySynchronizerDialog();

	void prepare();
	bool start();

	void autoPrepare();
	bool autoStart();

	QString side1Directory() const;
	QString side2Directory() const;

	QString side1DirectoryName() const;
	QString side2DirectoryName() const;

public slots:
	void setCloseOnCompletion(bool closeOnCompletion);
	void setTimedWarningOnCompletion(bool timedWarningOnCompletion);

	void appendExcludePattern(const QString &excludePattern);

signals:

protected slots:
	void onSynchronizerErrorTextChanged(const QString& message);
	void onSynchronizerProgressTextChanged(const QString& message);
	void onSynchronizerComplete();
	void onSynchronizerFailed();

	void onProgressChanged(int percentCompleteFile, int remainingFilesToCopy, int totalFilesToCopy);
	void onCurrentCopyFileChanged(const QString &currentCopyFile);

	void onCloseButtonClicked();

	void onPrepareButtonClicked();
	void onStartButtonClicked();

protected:
	void closeEvent(QCloseEvent *);

	QString side1ModifiedResultsString() const;
	QString side2ModifiedResultsString() const;

protected:
	QString side1Directory_;
	QString side2Directory_;

	QString side1DirectoryName_;
	QString side2DirectoryName_;

	QLabel *mainStatusLabel_;

	QProgressBar *singleFileProgressBar_;
	QLabel *singleFileLabel_;
	QProgressBar *overallTransferProgressBar_;
	QLabel *overallTransferLabel_;
	QLabel *currentCopyFileLabel_;

	QTextEdit *errorTextEdit_;
	QPushButton *errorCloseButton_;
	QTextEdit *progressTextEdit_;
	QTextEdit *fileListingEdit_;
	AMVerticalStackWidget *feedbackStackWidget_;

	AMDirectorySynchronizer *synchronizer_;
	AMRecursiveDirectoryCompare::DirectoryCompareResult lastCompareResult_;

	QPushButton *prepareButton_;
	QPushButton *startButton_;

	bool successfulSync_;
	bool closeOnCompletion_;
	bool timedWarningOnCompletion_;
};

#endif // AMDIRECTORYSYNCHRONIZERDIALOG_H
