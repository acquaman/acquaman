#ifndef AMDIRECTORYSYNCHRONIZERDIALOG_H
#define AMDIRECTORYSYNCHRONIZERDIALOG_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>

#include "util/AMDirectorySynchronizer.h"
class AMDirectorySynchronizerDialog : public QWidget
{
	Q_OBJECT
private:
	QProgressBar* progressBar_;
	QTextEdit* progressTextEdit_;
	QTextEdit* errorTextEdit_;
	QPushButton* startButton_;
	AMDirectorySynchronizer* synchronizer_;
public:
	explicit AMDirectorySynchronizerDialog(QWidget *parent = 0);
	
signals:
	
public slots:
protected slots:
	void onSynchronizerErrorTextChanged(const QStringList& list);
	void onSynchronizerProgressTextChanges(const QStringList& list);
	void onSynchronizerComplete();
	void onStartButtonClicked();
	void onPercentageProgressChanged(int);

	
};

#endif // AMDIRECTORYSYNCHRONIZERDIALOG_H
