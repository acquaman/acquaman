#ifndef AMDIRECTORYSYNCHRONIZERDIALOG_H
#define AMDIRECTORYSYNCHRONIZERDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>

#include "util/AMDirectorySynchronizer.h"
class AMDirectorySynchronizerDialog : public QDialog
{
	Q_OBJECT
private:
	QProgressBar* progressBar_;
	QTextEdit* errorTextEdit_;
	QPushButton* errorCloseButton_;
	AMDirectorySynchronizer* synchronizer_;
	bool successfulSync_;
public:
	explicit AMDirectorySynchronizerDialog(QWidget *parent = 0);
	~AMDirectorySynchronizerDialog();
	bool start();
protected:
	void closeEvent(QCloseEvent *);
signals:
	
public slots:
protected slots:
	void onSynchronizerErrorTextChanged(const QString& message);
	void onSynchronizerComplete();
	void onSynchronizerFailed();
	void onPercentageProgressChanged(int);
	void onCloseButtonClicked();
};

#endif // AMDIRECTORYSYNCHRONIZERDIALOG_H
