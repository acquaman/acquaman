#include "AMDirectorySynchronizerDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCloseEvent>
AMDirectorySynchronizerDialog::AMDirectorySynchronizerDialog(QWidget *parent) :
	QDialog(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0, 100);
	errorTextEdit_ = new QTextEdit();
	errorTextEdit_->setReadOnly(true);
	errorTextEdit_->setVisible(false);
	errorCloseButton_ = new QPushButton("Close");
	errorCloseButton_->setVisible(false);

	mainLayout->addWidget(new QLabel("Acquaman is currently backing up your data. Please wait till complete..."));
	mainLayout->addWidget(progressBar_, Qt::AlignCenter);
	mainLayout->addWidget(errorTextEdit_);
	mainLayout->addWidget(errorCloseButton_, Qt::AlignCenter);

	setLayout(mainLayout);
	synchronizer_ = new AMDirectorySynchronizer("/home/sgm/Documents/CopyTests/Source", "/home/sgm/Documents/CopyTests/Destination");

	connect(synchronizer_, SIGNAL(copyCompleted()), this, SLOT(onSynchronizerComplete()));
	connect(synchronizer_, SIGNAL(copyFailed()), this, SLOT(onSynchronizerComplete()));
	connect(synchronizer_, SIGNAL(errorMessagesChanged(const QStringList&)), this, SLOT(onSynchronizerErrorTextChanged(const QStringList&)));
	connect(synchronizer_, SIGNAL(percentageProgressChanged(int)), this, SLOT(onPercentageProgressChanged(int)));
	connect(errorCloseButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
	setModal(true);
}

AMDirectorySynchronizerDialog::~AMDirectorySynchronizerDialog()
{
	synchronizer_->deleteLater();
}

void AMDirectorySynchronizerDialog::closeEvent(QCloseEvent *ce)
{
	if(synchronizer_->isRunning())
		ce->setAccepted(false);
	else
		ce->setAccepted(true);

}

void AMDirectorySynchronizerDialog::start()
{
	if(synchronizer_->start())
		show();
}

void AMDirectorySynchronizerDialog::onSynchronizerErrorTextChanged(const QStringList &list)
{
	errorTextEdit_->clear();
	for(int iMessage = 0; iMessage < list.count(); iMessage++)
	{
		errorTextEdit_->append(list.at(iMessage));
	}
}

void AMDirectorySynchronizerDialog::onSynchronizerComplete()
{
	setCursor(Qt::ArrowCursor);
	if(errorTextEdit_->toPlainText().length() != 0)
	{
		errorTextEdit_->setVisible(true);
		errorCloseButton_->setVisible(true);
	}
	else
		close();
}


void AMDirectorySynchronizerDialog::onPercentageProgressChanged(int value)
{
	progressBar_->setValue(value);
}

void AMDirectorySynchronizerDialog::onCloseButtonClicked()
{
	close();
}
