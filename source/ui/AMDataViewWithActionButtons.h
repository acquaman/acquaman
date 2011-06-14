#ifndef AMDATAVIEWWITHACTIONBUTTONS_H
#define AMDATAVIEWWITHACTIONBUTTONS_H

#include "ui/AMDataView.h"
#include "ui_AMDataViewActionsBar.h"

/// This class adds buttons for editing, comparing, and exporting the selected scans in an AMDataView.
class AMDataViewWithActionButtons : public AMDataView
{
	Q_OBJECT
public:
	explicit AMDataViewWithActionButtons(AMDatabase* database = AMDatabase::userdb(), QWidget *parent = 0);

signals:
	/// Emitted when the user attempts to open the selected scans
	void selectionActivated(const QList<QUrl>&);
	/// Emitted when the user attempts to open the selected scans in separate windows
	void selectionActivatedSeparateWindows(const QList<QUrl> &);
	/// Emitted when the user attempts to export the selected scans. (This action is (hopefully) completed elsewhere, so that we don't couple the AMDataView to the export system)
	void selectionExported(const QList<QUrl>&);


public slots:

protected slots:
	/// When the "open in same window" action happens
	void onCompareScansAction() {
		emit selectionActivated(selectedItems());
	}

	/// When the "open in separate window" action
	void onEditScansAction() {
		emit selectionActivatedSeparateWindows(selectedItems());
	}

	/// When the "export scans" action happens
	void onExportScansAction() {
		emit selectionExported(selectedItems());
	}

	/// When anything is double-clicked.  If more than 0 items are selected, we emit selectionActivated().
	void onDoubleClick();

	/// When the base class's selection changes. We might need to enable or disable some of the buttons.
	void onSelectionChanged();

protected:
	Ui::AMDataViewActionsBar* ui_;

};

#endif // AMDATAVIEWWITHACTIONBUTTONS_H
