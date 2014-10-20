#include "AMDirectorySynchronizerDialog.h"

#pragma clang diagnostic ignored "-Wunused-private-field"
#include <QCloseEvent>
#pragma clang diagnostic warning "-Wunused-private-field"

#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QApplication>

#include "util/AMSettings.h"
#include "ui/AMVerticalStackWidget.h"
#include "ui/util/AMMessageBoxWTimeout.h"
#include "util/AMThread.h"


AMDirectorySynchronizerDialog::AMDirectorySynchronizerDialog(const QString &side1Directory, const QString &side2Directory, const QString &side1DirectoryName, const QString &side2DirectoryName, QWidget *parent) :
	QDialog(parent)
{
	delaySeconds_ = 0;
	delayCountdown_ = 0;

	side1Directory_ = side1Directory;
	side2Directory_ = side2Directory;

	side1DirectoryName_ = side1DirectoryName;
	side2DirectoryName_ = side2DirectoryName;

	closeOnCompletion_ = false;
	timedWarningOnCompletion_ = false;
	lastCompareResult_ = AMRecursiveDirectoryCompare::NotYetRun;

	QVBoxLayout* mainLayout = new QVBoxLayout();

	singleFileProgressBar_ = new QProgressBar();
	singleFileProgressBar_->setRange(0, 100);
	singleFileLabel_ = new QLabel();

	overallTransferProgressBar_ = new QProgressBar();
	overallTransferProgressBar_->setRange(0, 100);
	overallTransferLabel_ = new QLabel();

	currentCopyFileLabel_ = new QLabel();

	errorTextEdit_ = new QTextEdit();
	errorTextEdit_->setReadOnly(true);
	errorCloseButton_ = new QPushButton("Close");
	errorCloseButton_->setVisible(false);
	successfulSync_ = false;

	progressTextEdit_ = new QTextEdit();
	progressTextEdit_->setReadOnly(true);

	fileListingEdit_ = new QTextEdit();
	fileListingEdit_->setReadOnly(true);

	feedbackStackWidget_ = new AMVerticalStackWidget();
	feedbackStackWidget_->addItem("Files", fileListingEdit_);
	feedbackStackWidget_->addItem("Progress", progressTextEdit_);
	feedbackStackWidget_->addItem("Errors", errorTextEdit_);
	feedbackStackWidget_->collapseItem(0);
	feedbackStackWidget_->collapseItem(1);
	feedbackStackWidget_->collapseItem(2);

	prepareButton_ = new QPushButton("Prepare");
	startButton_ = new QPushButton("Start");
	QHBoxLayout *buttonsHL = new QHBoxLayout();
	buttonsHL->addSpacing(0);
	buttonsHL->addWidget(prepareButton_);
	buttonsHL->addWidget(startButton_);

	QHBoxLayout *singleFileHL = new QHBoxLayout();
	singleFileHL->addWidget(singleFileProgressBar_, Qt::AlignCenter);
	QHBoxLayout *overallTransferHL = new QHBoxLayout();
	overallTransferHL->addWidget(overallTransferProgressBar_, Qt::AlignCenter);

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addLayout(singleFileHL, 0, 0, 1, 9, Qt::AlignCenter);
	gridLayout->addWidget(singleFileLabel_, 0, 10, 1, 1, Qt::AlignRight);
	gridLayout->addLayout(overallTransferHL, 1, 0, 1, 9, Qt::AlignCenter);
	gridLayout->addWidget(overallTransferLabel_, 1, 10, 1, 1, Qt::AlignRight);

	mainStatusLabel_ = new QLabel("Synchronize Directories, press Prepare or Start to continue.");
	mainLayout->addWidget(mainStatusLabel_);

	mainLayout->addLayout(gridLayout);
	mainLayout->addWidget(currentCopyFileLabel_);

	mainLayout->addLayout(buttonsHL);
	mainLayout->addWidget(feedbackStackWidget_);
	mainLayout->addWidget(errorCloseButton_, Qt::AlignCenter);


	setLayout(mainLayout);
	synchronizer_ = new AMDirectorySynchronizer(side1Directory_, side2Directory_);
	synchronizer_->setSide1DirectorName(side1DirectoryName_);
	synchronizer_->setSide2DirectorName(side2DirectoryName_);
	synchronizer_->setAllowSide1Creation(true);

	connect(synchronizer_, SIGNAL(copyCompleted()), this, SLOT(onSynchronizerComplete()));
	connect(synchronizer_, SIGNAL(copyFailed()), this, SLOT(onSynchronizerComplete()));
	connect(synchronizer_, SIGNAL(errorMessagesChanged(const QString&)), this, SLOT(onSynchronizerErrorTextChanged(const QString&)));
	connect(synchronizer_, SIGNAL(progressMessagesChanged(QString)), this, SLOT(onSynchronizerProgressTextChanged(QString)));
	connect(synchronizer_, SIGNAL(progressChanged(int,int,int)), this, SLOT(onProgressChanged(int,int,int)));
	connect(synchronizer_, SIGNAL(currentCopyFileChanged(QString)), this, SLOT(onCurrentCopyFileChanged(QString)));

	connect(errorCloseButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));

	connect(prepareButton_, SIGNAL(clicked()), this, SLOT(onPrepareButtonClicked()));
	connect(startButton_, SIGNAL(clicked()), this, SLOT(onStartButtonClicked()));

	startButton_->setDefault(false);
	errorCloseButton_->setDefault(false);
	prepareButton_->setDefault(true);

	setModal(true);
}

AMDirectorySynchronizerDialog::~AMDirectorySynchronizerDialog()
{
	synchronizer_->deleteLater();
}

QString AMDirectorySynchronizerDialog::side1Directory() const{
	return side1Directory_;
}

QString AMDirectorySynchronizerDialog::side2Directory() const{
	return side2Directory_;
}

QString AMDirectorySynchronizerDialog::side1DirectoryName() const{
	return side1DirectoryName_;
}

QString AMDirectorySynchronizerDialog::side2DirectoryName() const{
	return side2DirectoryName_;
}

void AMDirectorySynchronizerDialog::setCloseOnCompletion(bool closeOnCompletion){
	closeOnCompletion_ = closeOnCompletion;
}

void AMDirectorySynchronizerDialog::setTimedWarningOnCompletion(bool timedWarningOnCompletion){
	timedWarningOnCompletion_ = timedWarningOnCompletion;
}

void AMDirectorySynchronizerDialog::appendExcludePattern(const QString &excludePattern){
	synchronizer_->appendExcludePattern(excludePattern);
}

void AMDirectorySynchronizerDialog::closeEvent(QCloseEvent *ce)
{
	if(synchronizer_->isRunning())
		ce->setAccepted(false);
	else{
		ce->setAccepted(true);
		emit dialogCompleted();
		if(closeOnCompletion_)
			QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
	}

}

QString AMDirectorySynchronizerDialog::side1ModifiedResultsString() const{
	QString textString;

	textString.append("<br>Unique files:<br>");
	if(synchronizer_->comparator()->uniqueSide1Files().isEmpty())
		textString.append("<None><br>");
	for(int x = 0, size = synchronizer_->comparator()->uniqueSide1Files().count(); x < size; x++)
		textString.append(QString("%1<br>").arg(synchronizer_->comparator()->uniqueSide1Files().at(x)));
	textString.append("<br>Unique directories:<br>");
	if(synchronizer_->comparator()->uniqueSide1Directories().isEmpty())
		textString.append("<None><br>");
	for(int x = 0, size = synchronizer_->comparator()->uniqueSide1Directories().count(); x < size; x++)
		textString.append(QString("%1<br>").arg(synchronizer_->comparator()->uniqueSide1Directories().at(x)));
	textString.append("<br>Newer files:<br>");
	if(synchronizer_->comparator()->newerSide1Files().isEmpty())
		textString.append("<None><br>");
	for(int x = 0, size = synchronizer_->comparator()->newerSide1Files().count(); x < size; x++)
		textString.append(QString("%1<br>").arg(synchronizer_->comparator()->newerSide1Files().at(x)));

	return textString;
}

QString AMDirectorySynchronizerDialog::side2ModifiedResultsString() const{
	QString textString;

	textString.append("<br>Unique files:<br>");
	if(synchronizer_->comparator()->uniqueSide2Files().isEmpty())
		textString.append("<None><br>");
	for(int x = 0, size = synchronizer_->comparator()->uniqueSide2Files().count(); x < size; x++)
		textString.append(QString("%1<br>").arg(synchronizer_->comparator()->uniqueSide2Files().at(x)));
	textString.append("<br>Unique directories:<br>");
	if(synchronizer_->comparator()->uniqueSide2Directories().isEmpty())
		textString.append("<None><br>");
	for(int x = 0, size = synchronizer_->comparator()->uniqueSide2Directories().count(); x < size; x++)
		textString.append(QString("%1<br>").arg(synchronizer_->comparator()->uniqueSide2Directories().at(x)));
	textString.append("<br>Newer files:<br>");
	if(synchronizer_->comparator()->newerSide2Files().isEmpty())
		textString.append("<None><br>");
	for(int x = 0, size = synchronizer_->comparator()->newerSide2Files().count(); x < size; x++)
		textString.append(QString("%1<br>").arg(synchronizer_->comparator()->newerSide2Files().at(x)));

	return textString;
}

void AMDirectorySynchronizerDialog::prepare(){
	QApplication::setOverrideCursor(Qt::WaitCursor);
	mainStatusLabel_->setText("Preparing ...");

	prepareButton_->setEnabled(false);
	startButton_->setEnabled(false);

	singleFileProgressBar_->setMinimum(0);
	singleFileProgressBar_->setMaximum(0);
	overallTransferProgressBar_->setMinimum(0);
	overallTransferProgressBar_->setMaximum(0);

	fileListingEdit_->clear();
	errorTextEdit_->clear();
	progressTextEdit_->clear();

	feedbackStackWidget_->collapseItem(0);
	feedbackStackWidget_->collapseItem(1);
	feedbackStackWidget_->collapseItem(2);

	qRegisterMetaType<AMRecursiveDirectoryCompare::DirectoryCompareResult>("DirectorCompareResult");
	connect(synchronizer_, SIGNAL(prepared(AMRecursiveDirectoryCompare::DirectoryCompareResult)), this, SLOT(onPrepared(AMRecursiveDirectoryCompare::DirectoryCompareResult)));

	AMThread *prepareThread = new AMThread();
	connect(synchronizer_, SIGNAL(prepared(AMRecursiveDirectoryCompare::DirectoryCompareResult)), prepareThread, SLOT(onWorkerFinished()));
	prepareThread->setWorkerObject(synchronizer_);
	prepareThread->setInitialThread(thread());
	synchronizer_->moveToThread(prepareThread);
	prepareThread->start();
	QTimer::singleShot(0, synchronizer_, SLOT(prepare()));
}

void AMDirectorySynchronizerDialog::onPrepared(AMRecursiveDirectoryCompare::DirectoryCompareResult comparisonResult){
	lastCompareResult_ = comparisonResult;

	overallTransferProgressBar_->setMinimum(100);
	overallTransferProgressBar_->setMaximum(100);

	QString textString;
	switch(lastCompareResult_){
	case AMRecursiveDirectoryCompare::InvalidResult:
		textString.append(QString("<font color=\"Red\" size=\"5\">It looks like the synchronizer is already running. Cannot proceed.</font><br><br>").arg(side2DirectoryName_).arg(side1DirectoryName_));
		errorTextEdit_->append(textString);
		break;
	case AMRecursiveDirectoryCompare::FullyMatchingResult:
		textString.append(QString("<font color=\"Green\" size=\"5\">Everything matches, no need to synchronize (%1 and %2).</font><br><br>").arg(side2DirectoryName_).arg(side1DirectoryName_));
		progressTextEdit_->setText(textString);
		break;
	case AMRecursiveDirectoryCompare::Side1ModifiedResult:
		textString.append(QString("Side %1 has been modified.<br><br>").arg(side1DirectoryName_));
		textString.append(QString("<font color=\"Green\" size=\"5\"><i>Start</i> to copy files from %1 to %2</font><br><br>").arg(side1DirectoryName_).arg(side2DirectoryName_));

		textString.append(side1ModifiedResultsString());
		fileListingEdit_->setText(textString);
		break;
	case AMRecursiveDirectoryCompare::Side2ModifiedResult:
		textString.append(QString("Side %1 has been modified.<br><br>").arg(side2DirectoryName_));
		textString.append(QString("<font color=\"Green\" size=\"5\"><i>Start</i> to copy files from %1 to %2</font><br><br>").arg(side2DirectoryName_).arg(side1DirectoryName_));

		textString.append(side2ModifiedResultsString());
		fileListingEdit_->setText(textString);
		break;
	case AMRecursiveDirectoryCompare::BothSidesModifiedResult:
		textString.append(QString("<font color=\"Red\" size=\"5\">Both sides (%1 and %2) have been modified. Cannot proceed.</font><br><br>").arg(side2DirectoryName_).arg(side1DirectoryName_));
		errorTextEdit_->append(textString);

		fileListingEdit_->append(QString("Side %1 has been modified.<br><br>").arg(side1DirectoryName_));
		fileListingEdit_->append(side1ModifiedResultsString());
		fileListingEdit_->append(QString("Side %1 has been modified.<br><br>").arg(side2DirectoryName_));
		fileListingEdit_->append(side2ModifiedResultsString());

		break;
	case AMRecursiveDirectoryCompare::Side1DoesNotExistResult:
	case AMRecursiveDirectoryCompare::Side2DoesNotExistResult:
	case AMRecursiveDirectoryCompare::NeitherSideExistsResult:
	case AMRecursiveDirectoryCompare::BothSidesAreFiles:
	case AMRecursiveDirectoryCompare::Side1IsFile:
	case AMRecursiveDirectoryCompare::Side2IsFile:
	case AMRecursiveDirectoryCompare::UnableToDetermineResult:
	default:
		if(progressTextEdit_->toPlainText().length() != 0){
			QString progressString = progressTextEdit_->toPlainText();
			progressString.prepend("<font color=\"Green\" size=\"3\">");
			progressString.append("</font>");
			progressTextEdit_->setText(progressString);
		}
		else if(errorTextEdit_->toPlainText().length() != 0){
			QString errorString = errorTextEdit_->toPlainText();
			errorString.prepend("<font color=\"Red\" size=\"3\">");
			errorString.append("</font>");
			progressTextEdit_->setText(errorString);
		}
		break;
	}

	prepareButton_->setEnabled(true);
	startButton_->setEnabled(true);

	if(fileListingEdit_->toPlainText().length() != 0)
		feedbackStackWidget_->expandItem(0);

	if(progressTextEdit_->toPlainText().length() != 0)
		feedbackStackWidget_->expandItem(1);

	if(errorTextEdit_->toPlainText().length() != 0)
	{
		feedbackStackWidget_->expandItem(2);
		errorCloseButton_->setVisible(true);
		startButton_->setEnabled(false);
		prepareButton_->setIcon(QIcon(":/22x22/list-remove-2.png"));

		prepareButton_->setDefault(false);
		startButton_->setDefault(false);
		errorCloseButton_->setDefault(true);

		mainStatusLabel_->setText("Prepare failed. Press Close to end program.");
	}
	else{
		prepareButton_->setIcon(QIcon(":greencheckmark.png"));

		prepareButton_->setDefault(false);
		errorCloseButton_->setDefault(false);
		startButton_->setDefault(true);

		mainStatusLabel_->setText("Prepare succeeded. Press Start to continue.");
	}
	QApplication::restoreOverrideCursor();

	emit prepared();
}

bool AMDirectorySynchronizerDialog::start()
{
	connect(this, SIGNAL(prepared()), this, SLOT(onStartPrepared()));
	prepare();
	return true;
}

void AMDirectorySynchronizerDialog::onStartPrepared(){
	mainStatusLabel_->setText("Synchronzing directories. This may take some time, please wait ...");

	prepareButton_->setEnabled(false);
	startButton_->setEnabled(false);

	feedbackStackWidget_->collapseItem(1);

	synchronizer_->start();
}

void AMDirectorySynchronizerDialog::autoPrepare(){
	prepare();
	exec();
}

bool AMDirectorySynchronizerDialog::autoStart(){
	start();
	exec();
	return successfulSync_;
}

void AMDirectorySynchronizerDialog::delayedStart(double delaySeconds){
	prepareButton_->setEnabled(false);
	startButton_->setEnabled(false);
	delaySeconds_ = delaySeconds;
	mainStatusLabel_->setText(QString("Synchronization will start in %1 seconds.").arg(delaySeconds_));
	delayCountdownTimer_ = new QTimer();
	connect(delayCountdownTimer_, SIGNAL(timeout()), this, SLOT(onDelayCountdown()));
	delayCountdownTimer_->start(1000);

	int delayTimeMS = 1000*delaySeconds;
	QTimer::singleShot(delayTimeMS, this, SLOT(onStartButtonClicked()));
}

void AMDirectorySynchronizerDialog::onSynchronizerErrorTextChanged(const QString &message)
{
	errorTextEdit_->append(message);
}

void AMDirectorySynchronizerDialog::onSynchronizerProgressTextChanged(const QString &message){
	progressTextEdit_->append(message);
}

void AMDirectorySynchronizerDialog::onSynchronizerComplete()
{
	setCursor(Qt::ArrowCursor);
	successfulSync_ = true;
	if(errorTextEdit_->toPlainText().length() != 0)
	{
		feedbackStackWidget_->expandItem(2);
		errorCloseButton_->setVisible(true);

		startButton_->setIcon(QIcon(":/22x22/list-remove-2.png"));

		prepareButton_->setDefault(false);
		startButton_->setDefault(false);
		errorCloseButton_->setDefault(true);

		mainStatusLabel_->setText("Synchronization failed. Press Close to end program.");
	}
	else{
		startButton_->setIcon(QIcon(":greencheckmark.png"));

		mainStatusLabel_->setText("Synchronization succeeded.");

		if(timedWarningOnCompletion_){
			AMMessageBoxWTimeout box(30000);
			box.setWindowTitle("Synchronization Complete.");
			box.setText("Synchronization Complete.");
			if(lastCompareResult_ == AMRecursiveDirectoryCompare::Side1ModifiedResult)
				box.setInformativeText(QString("Acquaman has successfully copied files from %1 to %2.").arg(side1DirectoryName_).arg(side2DirectoryName_));
			else if(lastCompareResult_ == AMRecursiveDirectoryCompare::Side2ModifiedResult || lastCompareResult_ == AMRecursiveDirectoryCompare::Side1DoesNotExistResult)
				box.setInformativeText(QString("Acquaman has successfully copied files from %1 to %2.").arg(side2DirectoryName_).arg(side1DirectoryName_));

			QPushButton *acknowledgeButton_ = new QPushButton("Ok");

			box.addButton(acknowledgeButton_, QMessageBox::AcceptRole);
			box.setDefaultButton(acknowledgeButton_);

			box.execWTimeout();
			close();
		}
		else
			QTimer::singleShot(1000, this, SLOT(close()));
	}
}

void AMDirectorySynchronizerDialog::onSynchronizerFailed()
{
	setCursor(Qt::ArrowCursor);
	successfulSync_ = false;
	if(errorTextEdit_->toPlainText().length() != 0)
	{
		feedbackStackWidget_->expandItem(2);
		errorCloseButton_->setVisible(true);

		startButton_->setIcon(QIcon(":/22x22/list-remove-2.png"));

		prepareButton_->setDefault(false);
		startButton_->setDefault(false);
		errorCloseButton_->setDefault(true);

		mainStatusLabel_->setText("Synchronization failed. Press Close to end program.");
	}	
	else{
		startButton_->setIcon(QIcon(":/22x22/list-remove-2.png"));

		mainStatusLabel_->setText("Synchronization failed. Press Close to end program.");

		if(timedWarningOnCompletion_){
			AMMessageBoxWTimeout box(30000);
			box.setWindowTitle("Synchronization Failed.");
			box.setText("Synchronization Failed.");
			if(lastCompareResult_ == AMRecursiveDirectoryCompare::Side1ModifiedResult)
				box.setInformativeText(QString("Acquaman failed to copy files from %1 to %2.").arg(side1DirectoryName_).arg(side2DirectoryName_));
			else if(lastCompareResult_ == AMRecursiveDirectoryCompare::Side2ModifiedResult || lastCompareResult_ == AMRecursiveDirectoryCompare::Side1DoesNotExistResult)
				box.setInformativeText(QString("Acquaman failed to copy files from %1 to %2.").arg(side2DirectoryName_).arg(side1DirectoryName_));

			QPushButton *acknowledgeButton_ = new QPushButton("Ok");

			box.addButton(acknowledgeButton_, QMessageBox::AcceptRole);
			box.setDefaultButton(acknowledgeButton_);

			box.execWTimeout();
			close();
		}
		else
			QTimer::singleShot(1000, this, SLOT(close()));
	}
}

void AMDirectorySynchronizerDialog::onProgressChanged(int percentCompleteFile, int remainingFilesToCopy, int totalFilesToCopy){
	overallTransferProgressBar_->setRange(0, totalFilesToCopy);

	singleFileProgressBar_->setValue(percentCompleteFile);
	singleFileLabel_->setText(QString("%1%").arg(percentCompleteFile));
	overallTransferProgressBar_->setValue(totalFilesToCopy-remainingFilesToCopy);
	overallTransferLabel_->setText(QString("%1 of %2").arg(totalFilesToCopy-remainingFilesToCopy).arg(totalFilesToCopy));
}

void AMDirectorySynchronizerDialog::onCurrentCopyFileChanged(const QString &currentCopyFile){
	if(currentCopyFile.isEmpty())
		currentCopyFileLabel_->setText("");
	else
		currentCopyFileLabel_->setText(QString("Copying: %1").arg(currentCopyFile));
}

void AMDirectorySynchronizerDialog::onCloseButtonClicked()
{
	close();
}

void AMDirectorySynchronizerDialog::onPrepareButtonClicked(){
	prepare();
}

void AMDirectorySynchronizerDialog::onStartButtonClicked(){
	start();
}

void AMDirectorySynchronizerDialog::onDelayCountdown(){
	delayCountdown_++;
	if(delaySeconds_-delayCountdown_ < 1.0)
		delayCountdownTimer_->stop();
	mainStatusLabel_->setText(QString("Synchronization will start in %1 seconds.").arg(delaySeconds_-delayCountdown_));
}
