#ifndef SXRMBCHOOSEDATAFOLDERDIALOG_H
#define SXRMBCHOOSEDATAFOLDERDIALOG_H

#include <QDialog>

#include "util/AMSettings.h"

class QLabel;
class QFileDialog;
class QCheckBox;
class QDir;
class QPushButton;
class QComboBox;

/// This class provides convience for getting the user data folder upon startup.  It provides a convience method of just entering in the proposal number and also a more general file dialog selection.
class SXRMBChooseDataFolderDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds the dialog.
	SXRMBChooseDataFolderDialog(const QString &dataFolder, QWidget *parent = 0);
	/// Destructor.
	virtual ~SXRMBChooseDataFolderDialog();

	/// Static method that makes and pops up the dialog.  Returns whether it was successful at updating the user settings or not.
	static bool getDataFolder(QWidget *parent = 0);
	/// Returns the current path.
	QString filePath() const { return folder_; }

protected slots:
	/// Slot that does the work of opening a file dialog and getting the new file path.
	void getFilePath();
	/// Slot that handles when the path text changes.
	void onTextChanged(const QString &text);

protected:
	/// Method that checks whether the input is valid or not.  Takes into account whether in advanced or not-advanced mode.
	QString pathStatus(const QString &path) const;

	/// The line edit that holds the proposal number or the path to a folder.
	QComboBox *path_;
	/// The check box for the advanced choice.
	QCheckBox *advancedCheck_;
	/// The actual path stored as a string.
	QString folder_;
	/// The list of all the folders in the users folder.
	QStringList directories_;
	/// The okay button.
	QPushButton *okButton_;
	/// The path status label.
	QLabel *pathStatusLabel_;
};

#endif // SXRMBCHOOSEDATAFOLDERDIALOG_H
