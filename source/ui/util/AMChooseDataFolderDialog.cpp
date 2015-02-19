#include "AMChooseDataFolderDialog.h"

#include <QLabel>
#include <QFileDialog>
#include <QStringList>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QPushButton>
#include <QStringBuilder>
#include <QComboBox>

AMChooseDataFolderDialog::AMChooseDataFolderDialog(const QString &dataFolder, const QString &localPath, const QString &remotePath, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Choose Data Ouput Destination");

	folder_ = dataFolder;

	QLabel *explanation = new QLabel("Enter the proposal number or the absolute path to where you want your data to be saved.");
	path_ = new QComboBox;
	path_->setEditable(true);

	if (folder_.contains("/users")){

		QString tempFolder = folder_;
		tempFolder = tempFolder.left(tempFolder.indexOf(QRegExp("/users"))+6);
		QDir dir(tempFolder);

		directories_ = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
	}

	path_->addItems(directories_);

	pathStatusLabel_ = new QLabel;
	QFont pathFont(this->font());
	pathFont.setItalic(true);
	pathStatusLabel_->setFont(pathFont);

	advancedCheck_ = new QCheckBox("Advanced");
	QToolButton *folderButton = new QToolButton;
	folderButton->setIcon(QIcon(":/22x22/folder.png"));
	okButton_ = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Okay");
	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");
	QString shortFormPath = "";

	if (folder_.contains(localPath) || folder_.contains(remotePath)){

		shortFormPath = folder_;
		shortFormPath = shortFormPath.remove(QRegExp(QString("%1|%2").arg(localPath).arg(remotePath)));
		shortFormPath = shortFormPath.remove("/userData");
		shortFormPath = shortFormPath.remove("/");
	}

	connect(folderButton, SIGNAL(clicked()), this, SLOT(getFilePath()));
	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(path_, SIGNAL(editTextChanged(QString)), this, SLOT(onTextChanged(QString)));
	connect(advancedCheck_, SIGNAL(toggled(bool)), folderButton, SLOT(setEnabled(bool)));

	advancedCheck_->setChecked(shortFormPath.isEmpty());
	folderButton->setEnabled(shortFormPath.isEmpty());

	// If small form could not be resolved, then just use a full path.  Otherwise, set the folder text to the short form.
	if (shortFormPath.isEmpty())
		shortFormPath = folder_;

	else
		folder_ = shortFormPath;

	path_->setEditText(shortFormPath);

	QHBoxLayout *lineEditLayout = new QHBoxLayout;
	lineEditLayout->addWidget(path_);
	lineEditLayout->addWidget(pathStatusLabel_);
	lineEditLayout->addWidget(folderButton);
	lineEditLayout->addWidget(advancedCheck_);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton_);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(explanation);
	layout->addLayout(lineEditLayout);
	layout->addLayout(buttonLayout);

	setLayout(layout);
}

// Static member
//////////////////////////////////////

bool AMChooseDataFolderDialog::getDataFolder(const QString &localRootDirectory, const QString &remoteRootDirectory, const QString &destination, const QStringList &extraDestinations, QWidget *parent)
{
	AMUserSettings::load();

	AMChooseDataFolderDialog dialog(AMUserSettings::userDataFolder, QString("%1/%2").arg(localRootDirectory).arg(destination), QString("%1/%2").arg(remoteRootDirectory).arg(destination), parent);
	dialog.exec();

	if (dialog.result() == QDialog::Accepted){

		if (!dialog.isFullPath()){

			QString dialogInput = dialog.filePath();
			QFileInfo remoteFullPath(QString("%1/%2/%3").arg(remoteRootDirectory).arg(destination).arg(dialogInput));
			bool isFirstTimeUser = !remoteFullPath.exists();

			if (!remoteFullPath.exists()){

				QDir newPath(QString("%1/%2").arg(remoteRootDirectory).arg(destination));
				newPath.mkpath(QString("%1/userData").arg(dialogInput));

				foreach (QString newExtraDestination, extraDestinations)
					newPath.mkpath(QString("%1/%2").arg(dialogInput).arg(newExtraDestination));
			}

			if (dialog.isFullPath()){

				AMUserSettings::removeRemoteDataFolderEntry();
				AMUserSettings::userDataFolder = dialogInput;
				AMUserSettings::save();
			}

			else if (isFirstTimeUser){

				AMUserSettings::userDataFolder = QString("%1/%2/%3").arg(remoteRootDirectory).arg(destination).arg(dialogInput);
				AMUserSettings::remoteDataFolder = "";
				AMUserSettings::save(true);
			}

			else {

				AMUserSettings::userDataFolder = QString("%1/%2/%3/userData").arg(localRootDirectory).arg(destination).arg(dialogInput);
				AMUserSettings::remoteDataFolder = QString("%1/%2/%3/userData").arg(remoteRootDirectory).arg(destination).arg(dialogInput);
				AMUserSettings::save();
			}
		}

		return true;
	}

	else
		return false;
}

//////////////////////////////////////

void AMChooseDataFolderDialog::onTextChanged(const QString &text)
{
	QPalette palette(this->palette());
	QPalette pathPalette(this->palette());
	QString pathStatusString = pathStatus(text);

	okButton_->setDisabled(pathStatusString == "Absolute path does not exist!");

	if (pathStatusString == "Absolute path does not exist!"){

		palette.setColor(QPalette::Text, Qt::red);
		pathPalette.setColor(QPalette::WindowText, Qt::darkRed);
	}

	else if (pathStatusString.contains("Valid") || pathStatusString.contains("okay"))
		pathPalette.setColor(QPalette::WindowText, Qt::darkGreen);

	pathStatusLabel_->setText(pathStatusString);
	pathStatusLabel_->setPalette(pathPalette);
	path_->setPalette(palette);
	folder_ = text;
}

QString AMChooseDataFolderDialog::pathStatus(const QString &path) const
{
	if (advancedCheck_->isChecked())
		return QFileInfo(path).exists() ? "Absolute path okay!" : "Absolute path does not exist!";

	else if (isProposalNumber(path))
		return "Valid proposal number!";

	else if (directories_.contains(path))
		return "Valid existing experiment!";

	else
		return "New experiment!";
}

bool AMChooseDataFolderDialog::isProposalNumber(const QString &path) const
{
	return path.contains(QRegExp("^\\d{2,2}-\\d{4,4}$"));
}

void AMChooseDataFolderDialog::getFilePath()
{
	QString path = folder_;
	path.chop(1);
	path = path.remove("/userData");
	QString dir = QFileDialog::getExistingDirectory(this, "Choose a destination folder for your data.", path, QFileDialog::ShowDirsOnly);

	if (!dir.isEmpty())
		path_->setEditText(dir);
}
