#ifndef AMCHOOSEDATAFOLDERDIALOG_H
#define AMCHOOSEDATAFOLDERDIALOG_H

#include <QDialog>

#include "util/AMSettings.h"

#include <QCheckBox>

class QLabel;
class QFileDialog;
class QDir;
class QPushButton;
class QToolButton;
class QComboBox;

/// This class provides a dialog to setup the local and remote paths for AM applications.
class AMChooseDataFolderDialog : public QDialog
{
	Q_OBJECT

public:
	/// Static method that makes and pops up the dialog.  Returns whether it was successful at updating the user settings or not.
	static bool getDataFolder(const QString &localRootDirectory, const QString &remoteRootDirectory, const QString &dataDirectory, const QStringList &extraDataDirectory = QStringList(), QWidget *parent = 0);

	/// Constructor.  Builds the dialog.  Requires a folder to know where it should be looking.
	AMChooseDataFolderDialog(const QString &dataFolder, const QString &localRootDirectory, const QString &remoteRootDirectory, const QString &dataDirectory, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMChooseDataFolderDialog(){}

	/// Returns the current path.
	QString filePath() const { return folder_; }
	/// Returns the state of the advanced check box.
	bool isFullPath() const;

protected slots:
	/// Slot that does the work of opening a file dialog and getting the new file path.
	void getFilePath();
	/// Slot that handles when the path text changes.
	void onTextChanged(const QString &text);

protected:
	/// Method that checks whether the input is valid or not.  Takes into account whether in advanced or not-advanced mode.
	QString pathStatus(const QString &path) const;
	/// Determines whether the given string is the format of a proposal number.
	bool isProposalNumber(const QString &path) const;

	/// The line edit that holds the proposal number or the path to a folder.
	QComboBox *pathComboBox_;
	/// The actual path stored as a string.
	QString folder_;
	/// The list of all the folders in the users folder.
	QStringList directories_;
	/// The folder button
	QToolButton *folderButton_;
	/// The okay button.
	QPushButton *okButton_;
	/// The path status label.
	QLabel *pathStatusLabel_;
};

#endif // AMCHOOSEDATAFOLDERDIALOG_H
