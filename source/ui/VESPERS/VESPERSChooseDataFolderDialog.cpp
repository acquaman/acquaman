#include "VESPERSChooseDataFolderDialog.h"

#include "application/VESPERS/VESPERS.h"

#include <QLabel>
#include <QFileDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QStringList>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QPushButton>
#include <QStringBuilder>

 VESPERSChooseDataFolderDialog::~VESPERSChooseDataFolderDialog(){}
VESPERSChooseDataFolderDialog::VESPERSChooseDataFolderDialog(const QString &dataFolder, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Choose Data Ouput Destination");

	folder_ = dataFolder;

	QLabel *explanation = new QLabel("Enter the proposal number or the absolute path to where you want your data to be saved.");
	path_ = new QLineEdit;
	advancedCheck_ = new QCheckBox("Advanced");
	QToolButton *folderButton = new QToolButton;
	folderButton->setIcon(QIcon(":/22x22/folder.png"));
	okButton_ = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Okay");
	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");
	QString path = VESPERS::getProposalNumber(folder_);

	connect(folderButton, SIGNAL(clicked()), this, SLOT(getFilePath()));
	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(path_, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
	connect(advancedCheck_, SIGNAL(toggled(bool)), folderButton, SLOT(setEnabled(bool)));

	advancedCheck_->setChecked(path.isEmpty());
	folderButton->setEnabled(path.isEmpty());

	if (path.isEmpty())
		path = folder_;

	path_->setText(path);
	okButton_->setEnabled(pathOk(advancedCheck_->isChecked() ? folder_ : VESPERS::getProposalNumber(folder_)));

	QHBoxLayout *lineEditLayout = new QHBoxLayout;
	lineEditLayout->addWidget(path_);
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

bool VESPERSChooseDataFolderDialog::getDataFolder(QWidget *parent)
{
	AMUserSettings::load();
	VESPERSChooseDataFolderDialog dialog(AMUserSettings::userDataFolder, parent);

	dialog.exec();

	if (dialog.result() == QDialog::Accepted){

		QString dir = dialog.filePath();

		if (dir.contains(QRegExp("^\\d{2,2}-\\d{4,4}$"))){

			QFileInfo info("/nas/vespers/users/" % dir);

			if (!info.exists()){

				QDir newPath("/nas/vespers/users");
				newPath.mkdir(dir);
				newPath.cd(dir);
			}

			dir = "/nas/vespers/users/" % dir;
		}

		if (!dir.endsWith("/"))
			dir += "/";

		if (!dir.contains("CCD Images")){

			QDir makeNewDir(dir);
			makeNewDir.rename("CCD Images", "XRD Images");
		}

		if (!dir.contains("XRD Images")){

			QDir makeNewDir(dir);
			makeNewDir.mkdir("XRD Images");
		}

		if (!dir.contains("userData")){

			QDir makeNewDir(dir);
			makeNewDir.mkdir("userData");
			makeNewDir.cd("userData");
			dir = makeNewDir.absolutePath() + "/";
		}

		if (dir.compare(AMUserSettings::userDataFolder) != 0){

			AMUserSettings::userDataFolder = dir;
			AMUserSettings::save();
		}

		return true;
	}

	else
		return false;
}

//////////////////////////////////////

void VESPERSChooseDataFolderDialog::onTextChanged(const QString &text)
{
	QPalette palette(this->palette());

	if (!pathOk(text)){

		okButton_->setEnabled(false);
		palette.setColor(QPalette::Text, Qt::red);
	}

	else
		okButton_->setEnabled(true);

	path_->setPalette(palette);
	folder_ = text;
}

bool VESPERSChooseDataFolderDialog::pathOk(const QString &path) const
{
	if (advancedCheck_->isChecked())
		return QFileInfo(path).exists();

	else
		return !VESPERS::getProposalNumber(path).isEmpty();
}

void VESPERSChooseDataFolderDialog::getFilePath()
{
	QString path = folder_;
	path.chop(1);
	path = path.remove("/userData");
	QString dir = QFileDialog::getExistingDirectory(this, "Choose a destination folder for your data.", path, QFileDialog::ShowDirsOnly);

	if (!dir.isEmpty())
		path_->setText(dir);
}
