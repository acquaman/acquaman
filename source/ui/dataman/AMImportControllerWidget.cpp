#include "ui/dataman/AMImportControllerWidget.h"


AMImportControllerWidget::AMImportControllerWidget(QWidget *parent)
	: QWidget(parent)
{
	setupUi();
	connectSignals();
	thumbnailViewer_ = new AMThumbnailScrollWidget();
	verticalLayoutLeft->insertWidget(0, thumbnailViewer_);

	runEdit_ = new AMRunSelector(AMDatabase::database("user"));
	gridLayout->addWidget(runEdit_, 3, 2);
}

AMThumbnailScrollWidget *AMImportControllerWidget::thumbnailViewer()
{
	return thumbnailViewer_;
}

AMRunSelector *AMImportControllerWidget::runEdit()
{
	return runEdit_;
}

QProgressBar *AMImportControllerWidget::progressBar()
{
	return progressBar_;
}

void AMImportControllerWidget::setName(const QString &name)
{
	nameEdit_->setText(name);
}

void AMImportControllerWidget::setNumber(int number)
{
	numberEdit_->setValue(number);
}

void AMImportControllerWidget::setDateTime(QDateTime dateTime)
{
	dateTimeEdit_->setDateTime(dateTime);
}

void AMImportControllerWidget::setProgressLabel(const QString &progress)
{
	progressLabel_->setText(progress);
}

void AMImportControllerWidget::setLoadingStatus(const QString &status)
{
	loadingStatusLabel_->setText(status);
	if(status == "Could not load this file.")
		loadingStatusLabel_->setStyleSheet("color: red;");
	else
		loadingStatusLabel_->setStyleSheet("color: black");
}

void AMImportControllerWidget::addFormat(const QString &format)
{
	formatComboBox_->addItem(format);
}

QString AMImportControllerWidget::name()
{
	return nameEdit_->text();
}

int AMImportControllerWidget::number()
{
	return numberEdit_->value();
}

QDateTime AMImportControllerWidget::dateTime()
{
	return dateTimeEdit_->dateTime();
}

int AMImportControllerWidget::sampleId()
{
	return sampleEdit_->value();
}

QString AMImportControllerWidget::format()
{
	return formatComboBox_->currentText();
}

int AMImportControllerWidget::selectedFormatIndex()
{
	return formatComboBox_->currentIndex();
}

bool AMImportControllerWidget::isIncludeNameChecked()
{
	return checkName_->isChecked();
}

bool AMImportControllerWidget::isIncludeNumberChecked()
{
	return checkNumber_->isChecked();
}

bool AMImportControllerWidget::isIncludeDataTimeChecked()
{
	return checkDateTime_->isChecked();
}

void AMImportControllerWidget::setupUi()
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMImportControllerWidget"));
	resize(642, 380);
	setMinimumSize(QSize(0, 380));
	verticalLayout_3 = new QVBoxLayout(this);
	verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
	AMImportControllerWidgetFrame1 = new QFrame(this);
	AMImportControllerWidgetFrame1->setObjectName(QString::fromUtf8("AMImportControllerWidgetFrame1"));
	AMImportControllerWidgetFrame1->setStyleSheet(QString::fromUtf8("QFrame#AMImportControllerWidgetFrame1 {\n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(206, 206, 206, 255), stop:0.502513 rgba(188, 188, 188, 255), stop:1 rgba(168, 168, 168, 255));\n"
"\n"
"border: 1px solid rgb(168,168,168);\n"
"}\n"
""));
	AMImportControllerWidgetFrame1->setFrameShape(QFrame::StyledPanel);
	horizontalLayout_2 = new QHBoxLayout(AMImportControllerWidgetFrame1);
	horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
	verticalLayoutLeft = new QVBoxLayout();
	verticalLayoutLeft->setSpacing(1);
	verticalLayoutLeft->setObjectName(QString::fromUtf8("verticalLayoutLeft"));
	verticalSpacer_2 = new QSpacerItem(1, 5, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

	verticalLayoutLeft->addItem(verticalSpacer_2);

	progressBar_ = new QProgressBar(AMImportControllerWidgetFrame1);
	progressBar_->setObjectName(QString::fromUtf8("progressBar"));
	progressBar_->setValue(24);

	verticalLayoutLeft->addWidget(progressBar_);

	progressLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	progressLabel_->setObjectName(QString::fromUtf8("progressLabel"));
	progressLabel_->setAlignment(Qt::AlignCenter);

	verticalLayoutLeft->addWidget(progressLabel_);


	horizontalLayout_2->addLayout(verticalLayoutLeft);

	verticalLayoutRight = new QVBoxLayout();
	verticalLayoutRight->setSpacing(2);
	verticalLayoutRight->setObjectName(QString::fromUtf8("verticalLayoutRight"));
	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	formatLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	formatLabel_->setObjectName(QString::fromUtf8("formatLabel"));

	horizontalLayout->addWidget(formatLabel_);

	formatComboBox_ = new QComboBox(AMImportControllerWidgetFrame1);
	formatComboBox_->setObjectName(QString::fromUtf8("formatComboBox"));
	QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(formatComboBox_->sizePolicy().hasHeightForWidth());
	formatComboBox_->setSizePolicy(sizePolicy);

	horizontalLayout->addWidget(formatComboBox_);


	verticalLayoutRight->addLayout(horizontalLayout);

	loadingStatusLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	loadingStatusLabel_->setObjectName(QString::fromUtf8("loadingStatusLabel"));
	loadingStatusLabel_->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

	verticalLayoutRight->addWidget(loadingStatusLabel_);

	verticalSpacer = new QSpacerItem(2, 5, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

	verticalLayoutRight->addItem(verticalSpacer);

	checkInstructionLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	checkInstructionLabel_->setObjectName(QString::fromUtf8("checkInstructionLabel"));

	verticalLayoutRight->addWidget(checkInstructionLabel_);

	gridLayout = new QGridLayout();
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	checkName_ = new QCheckBox(AMImportControllerWidgetFrame1);
	checkName_->setObjectName(QString::fromUtf8("checkName"));
	checkName_->setChecked(true);

	gridLayout->addWidget(checkName_, 0, 0, 1, 1);

	nameLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	nameLabel_->setObjectName(QString::fromUtf8("nameLabel"));

	gridLayout->addWidget(nameLabel_, 0, 1, 1, 1);

	nameEdit_ = new QLineEdit(AMImportControllerWidgetFrame1);
	nameEdit_->setObjectName(QString::fromUtf8("nameEdit"));
	nameEdit_->setEnabled(false);

	gridLayout->addWidget(nameEdit_, 0, 2, 1, 1);

	checkNumber_ = new QCheckBox(AMImportControllerWidgetFrame1);
	checkNumber_->setObjectName(QString::fromUtf8("checkNumber"));
	checkNumber_->setChecked(true);

	gridLayout->addWidget(checkNumber_, 1, 0, 1, 1);

	numberLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	numberLabel_->setObjectName(QString::fromUtf8("numberLabel"));

	gridLayout->addWidget(numberLabel_, 1, 1, 1, 1);

	numberEdit_ = new QSpinBox(AMImportControllerWidgetFrame1);
	numberEdit_->setObjectName(QString::fromUtf8("numberEdit"));
	numberEdit_->setMaximum(999999999);

	gridLayout->addWidget(numberEdit_, 1, 2, 1, 1);

	checkDateTime_ = new QCheckBox(AMImportControllerWidgetFrame1);
	checkDateTime_->setObjectName(QString::fromUtf8("checkDateTime"));
	checkDateTime_->setChecked(true);

	gridLayout->addWidget(checkDateTime_, 2, 0, 1, 1);

	dateTimeLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	dateTimeLabel_->setObjectName(QString::fromUtf8("dateTimeLabel"));

	gridLayout->addWidget(dateTimeLabel_, 2, 1, 1, 1);

	dateTimeEdit_ = new QDateTimeEdit(AMImportControllerWidgetFrame1);
	dateTimeEdit_->setObjectName(QString::fromUtf8("dateTimeEdit"));
	dateTimeEdit_->setEnabled(false);
	dateTimeEdit_->setCalendarPopup(true);

	gridLayout->addWidget(dateTimeEdit_, 2, 2, 1, 1);

	checkRun_ = new QCheckBox(AMImportControllerWidgetFrame1);
	checkRun_->setObjectName(QString::fromUtf8("checkRun"));
	checkRun_->setEnabled(false);

	gridLayout->addWidget(checkRun_, 3, 0, 1, 1);

	runLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	runLabel_->setObjectName(QString::fromUtf8("runLabel"));

	gridLayout->addWidget(runLabel_, 3, 1, 1, 1);

	checkSample_ = new QCheckBox(AMImportControllerWidgetFrame1);
	checkSample_->setObjectName(QString::fromUtf8("checkSample"));
	checkSample_->setEnabled(false);

	gridLayout->addWidget(checkSample_, 4, 0, 1, 1);

	sampleLabel_ = new QLabel(AMImportControllerWidgetFrame1);
	sampleLabel_->setObjectName(QString::fromUtf8("sampleLabel"));

	gridLayout->addWidget(sampleLabel_, 4, 1, 1, 1);

	sampleEdit_ = new QSpinBox(AMImportControllerWidgetFrame1);
	sampleEdit_->setObjectName(QString::fromUtf8("sampleEdit"));
	sampleEdit_->setMinimum(-1);
	sampleEdit_->setMaximum(999999999);
	sampleEdit_->setValue(-1);

	gridLayout->addWidget(sampleEdit_, 4, 2, 1, 1);


	verticalLayoutRight->addLayout(gridLayout);


	horizontalLayout_2->addLayout(verticalLayoutRight);


	verticalLayout_3->addWidget(AMImportControllerWidgetFrame1);

	horizontalLayout_3 = new QHBoxLayout();
	horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
	cancelButton_ = new QPushButton(this);
	cancelButton_->setObjectName(QString::fromUtf8("cancelButton"));
	cancelButton_->setAutoDefault(true);

	horizontalLayout_3->addWidget(cancelButton_);

	horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout_3->addItem(horizontalSpacer);

	nextButton_ = new QPushButton(this);
	nextButton_->setObjectName(QString::fromUtf8("nextButton"));
	nextButton_->setAutoDefault(true);
	nextButton_->setDefault(true);
	nextButton_->setFlat(false);

	horizontalLayout_3->addWidget(nextButton_);

	applyAllButton_ = new QPushButton(this);
	applyAllButton_->setObjectName(QString::fromUtf8("applyAllButton"));
	applyAllButton_->setAutoDefault(true);

	horizontalLayout_3->addWidget(applyAllButton_);


	verticalLayout_3->addLayout(horizontalLayout_3);

	setWindowTitle(QApplication::translate("AMImportControllerWidget", "Reviewing files to import...", 0, QApplication::UnicodeUTF8));
	progressLabel_->setText(QApplication::translate("AMImportControllerWidget", "File 3 of 10", 0, QApplication::UnicodeUTF8));
	formatLabel_->setText(QApplication::translate("AMImportControllerWidget", "File Format:", 0, QApplication::UnicodeUTF8));
	loadingStatusLabel_->setText(QApplication::translate("AMImportControllerWidget", "Loaded successfully", 0, QApplication::UnicodeUTF8));
	checkInstructionLabel_->setText(QApplication::translate("AMImportControllerWidget", "Check to use values in the data file:", 0, QApplication::UnicodeUTF8));
	checkName_->setText(QString());
	nameLabel_->setText(QApplication::translate("AMImportControllerWidget", "Name:", 0, QApplication::UnicodeUTF8));
	checkNumber_->setText(QString());
	numberLabel_->setText(QApplication::translate("AMImportControllerWidget", "Number:", 0, QApplication::UnicodeUTF8));
	checkDateTime_->setText(QString());
	dateTimeLabel_->setText(QApplication::translate("AMImportControllerWidget", "Date / Time:", 0, QApplication::UnicodeUTF8));
	dateTimeEdit_->setDisplayFormat(QApplication::translate("AMImportControllerWidget", "MMM d (yyyy), h:mm AP", 0, QApplication::UnicodeUTF8));
	checkRun_->setText(QString());
	runLabel_->setText(QApplication::translate("AMImportControllerWidget", "Run:", 0, QApplication::UnicodeUTF8));
	checkSample_->setText(QString());
	sampleLabel_->setText(QApplication::translate("AMImportControllerWidget", "Sample:", 0, QApplication::UnicodeUTF8));
	cancelButton_->setText(QApplication::translate("AMImportControllerWidget", "Cancel", 0, QApplication::UnicodeUTF8));
	nextButton_->setText(QApplication::translate("AMImportControllerWidget", "Next", 0, QApplication::UnicodeUTF8));
	applyAllButton_->setText(QApplication::translate("AMImportControllerWidget", "Apply to All", 0, QApplication::UnicodeUTF8));
}

void AMImportControllerWidget::connectSignals()
{
	connect(nextButton_, SIGNAL(clicked()), this, SIGNAL(nextClicked()));
	connect(applyAllButton_, SIGNAL(clicked()), this, SIGNAL(nextClicked()));
	connect(cancelButton_, SIGNAL(clicked()), this, SIGNAL(cancelClicked()));
	connect(formatComboBox_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(selectedFormatChanged(int)));
	connect(checkName_, SIGNAL(clicked(bool)), this, SLOT(onNameIncludeCheckboxChecked(bool)));
	connect(checkNumber_, SIGNAL(clicked(bool)), this, SLOT(onNumberIncludeCheckboxChecked(bool)));
	connect(checkDateTime_, SIGNAL(clicked(bool)), this, SLOT(onDateTimeIncludeCheckboxChanged(bool)));
}


void AMImportControllerWidget::onNameIncludeCheckboxChecked(bool include)
{
	nameEdit_->setEnabled(include);
	emit includeNameChanged(include);
}

void AMImportControllerWidget::onNumberIncludeCheckboxChecked(bool include)
{
	numberEdit_->setEnabled(include);
	emit includeNumberChanged(include);
}

void AMImportControllerWidget::onDateTimeIncludeCheckboxChanged(bool include)
{
	dateTimeEdit_->setEnabled(include);
	emit includeDateTimeChanged(include);
}



