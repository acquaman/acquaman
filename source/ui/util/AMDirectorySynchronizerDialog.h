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
public:
	explicit AMDirectorySynchronizerDialog(QWidget *parent = 0);
	~AMDirectorySynchronizerDialog();
	void start();
protected:
	void closeEvent(QCloseEvent *);
signals:
	
public slots:
protected slots:
	void onSynchronizerErrorTextChanged(const QStringList& list);
	void onSynchronizerComplete();
	void onPercentageProgressChanged(int);
	void onCloseButtonClicked();
};

#endif // AMDIRECTORYSYNCHRONIZERDIALOG_H
