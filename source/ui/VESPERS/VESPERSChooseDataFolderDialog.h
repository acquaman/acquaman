#ifndef VESPERSCHOOSEDATAFOLDERDIALOG_H
#define VESPERSCHOOSEDATAFOLDERDIALOG_H

#include <QDialog>

#include "util/AMSettings.h"

class QLabel;
class QFileDialog;
class QLineEdit;
class QCheckBox;
class QDir;
class QPushButton;

/// This class provides convience for getting the user data folder upon startup.  It provides a convience method of just entering in the proposal number and also a more general file dialog selection.
class VESPERSChooseDataFolderDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds the dialog.
	VESPERSChooseDataFolderDialog(const QString &dataFolder, QWidget *parent = 0);

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
	bool pathOk(const QString &path) const;

	/// The line edit that holds the proposal number or the path to a folder.
	QLineEdit *path_;
	/// The check box for the advanced choice.
	QCheckBox *advancedCheck_;
	/// The actual path stored as a string.
	QString folder_;
	/// The okay button.
	QPushButton *okButton_;
};

#endif // VESPERSCHOOSEDATAFOLDERDIALOG_H
