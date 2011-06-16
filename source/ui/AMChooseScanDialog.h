#ifndef AMCHOOSESCANDIALOG_H
#define AMCHOOSESCANDIALOG_H

#include <QDialog>
#include "ui_AMChooseScanDialog.h"

class AMDataView;
class AMDatabase;

/// Provides a dialog that lets users choose one more scans from a database.
/*! The standard QDialog signals are provided: finished(int result), and either accepted() or rejected().  After receiving these signals [or, after exec() finishes, if using this as a modal dialog], you can use getSelectedScans() to see what the user selected.

   \note It is recommended to use this as a modeless dialog, although it will work as a window-modal or application-modal dialog.  The QA_DeleteOnClose widget attribute is turned OFF by default, so that the dialog will not be deleted automatically when the user dismisses it.  This allows you to call getSelectedScans() after the dialog has been closed.  For more information on using modal and modeless dialogs, see the QDialog documentation.
   */
class AMChooseScanDialog : public QDialog
{
	Q_OBJECT
public:
	/// Construct a dialog box for the user to select one or more scans from a database. After the dialog is accepted, you can use getSelectedScans() to access the selected scans.
	/*! \param title Text displayed at the top of the dialog box
	  \param prompt Longer, more detailed text used to provide instructions to the user
	  \param multipleSelectionsAllowed Set this true if multiple selections are ok; otherwise, enforces choosing a single scan
	  \param parent Parent widget for this QDialog
	  */
	AMChooseScanDialog(AMDatabase* db, const QString& title, const QString& prompt, bool multipleSelectionAllowed = false, QWidget *parent = 0);

	/// Access a list of the selected scans.  They are provided as amd (AcquaMan Database) URLs, in the format amd://databaseConnectionName/tableName/id.  (This is the same format we use throughout the Acquaman framework for drag-and-drop, specifying scans to open in scan editors, etc.)
	QList<QUrl> getSelectedScans() const;



signals:

public slots:

protected slots:
	/// called when the selection in the data-view changes. We update the status text and enable/disable the OK button according to the number of scans selected.
	void onSelectionChanged();
	/// Called when someone double-clicks in the AMDataView.  If anything is selected, this should register as an OK.
	void onDoubleClick();
	/// Called when the showRuns or the showExperiments button is clicked.  \c code = 1 for runs and 2 for experiments
	void onRunOrExperimentButtonClicked(int code);

protected:
	AMDataView* dataView_;
	Ui::AMChooseScanDialog* ui_;
	bool multipleSelectionAllowed_;
	AMDatabase* db_;

	/// Helper function: lookup the name, number, and dateTime of a scan from the database, based on a amd:// URL.  Returns an empty string in the event of a problem.
	QString getScanDetails(QUrl amdUrl);

};

#endif // AMCHOOSESCANDIALOG_H
