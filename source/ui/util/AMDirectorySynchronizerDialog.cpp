#include "AMDirectorySynchronizerDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
AMDirectorySynchronizerDialog::AMDirectorySynchronizerDialog(QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0, 100);
	errorTextEdit_ = new QTextEdit();
	errorTextEdit_->setReadOnly(true);
	progressTextEdit_ = new QTextEdit();
	progressTextEdit_->setReadOnly(true);
	startButton_ = new QPushButton("Start Copy");

	mainLayout->addWidget(startButton_, Qt::AlignCenter);
	mainLayout->addWidget(progressBar_, Qt::AlignCenter);

	QHBoxLayout* progressLayout = new QHBoxLayout();
	progressLayout->addWidget(errorTextEdit_);
	progressLayout->addWidget(progressTextEdit_);

	mainLayout->addLayout(progressLayout);
	setLayout(mainLayout);
	synchronizer_ = new AMDirectorySynchronizer("/home/sgm/Documents/CopyTests/Source", "/home/sgm/Documents/CopyTests/Destination");

	connect(synchronizer_, SIGNAL(copyCompleted()), this, SLOT(onSynchronizerComplete()));
	connect(synchronizer_, SIGNAL(copyFailed()), this, SLOT(onSynchronizerComplete()));
	connect(synchronizer_, SIGNAL(errorMessagesChanged(const QStringList&)), this, SLOT(onSynchronizerErrorTextChanged(const QStringList&)));
	connect(synchronizer_, SIGNAL(progressMessageChanged(const QStringList&)), this, SLOT(onSynchronizerProgressTextChanges(const QStringList&)));
	connect(synchronizer_, SIGNAL(percentageProgressChanged(int)), this, SLOT(onPercentageProgressChanged(int)));
	connect(startButton_, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));

	setWindowModality(Qt::WindowModal);

}

void AMDirectorySynchronizerDialog::onSynchronizerErrorTextChanged(const QStringList &list)
{
	errorTextEdit_->clear();
	for(int iMessage = 0; iMessage < list.count(); iMessage++)
	{
		errorTextEdit_->append(list.at(iMessage));
	}
}

void AMDirectorySynchronizerDialog::onSynchronizerProgressTextChanges(const QStringList &list)
{
	progressTextEdit_->clear();
	for(int iMessage = 0; iMessage < list.count(); iMessage++)
	{
		progressTextEdit_->append(list.at(iMessage));
	}
}

void AMDirectorySynchronizerDialog::onSynchronizerComplete()
{
	setCursor(Qt::ArrowCursor);
}

void AMDirectorySynchronizerDialog::onStartButtonClicked()
{
	if(synchronizer_->start())
		this->setCursor(Qt::WaitCursor);
}

void AMDirectorySynchronizerDialog::onPercentageProgressChanged(int value)
{
	progressBar_->setValue(value);
}
