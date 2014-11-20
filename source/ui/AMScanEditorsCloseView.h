#ifndef AMSCANEDITORSCLOSEVIEW_H
#define AMSCANEDITORSCLOSEVIEW_H

#include <QWidget>

class QTreeView;
class QStandardItemModel;
class QStandardItem;
class QPushButton;

class AMDatamanAppController;
class AMGenericScanEditor;

class AMScanEditorsCloseView : public QWidget
{
Q_OBJECT
public:
	AMScanEditorsCloseView(QStandardItem* scanEditorsParentItem, AMDatamanAppController *appController, AMGenericScanEditor *originator, QWidget *parent = 0);

	virtual ~AMScanEditorsCloseView();

signals:
	/// Requests that some scan editors be closed
	void closeRequested(QList<AMGenericScanEditor*> editorsToClose);

	/// Emitted when this widget is closed
	void closed();

protected slots:
	/// Selects all of the GSEs
	void onSelectAllButtonClicked();
	/// Selects all of the GSEs except the one we initiated from
	void onSelectAllOthersButtonClicked();
	/// Selects all of the saved GSEs
	void onSelectSavedButtonClicked();
	/// Selects all of the GSEs with multiple scans open
	void onSelectMultiplesButtonClicked();
	/// Selects all of the GSEs with a single scan open
	void onSelectSinglesButtonClicked();

	/// Handles the close button
	void onCloseButtonClicked();
	/// Handles the cancel button
	void onCancelButtonClicked();

protected:
	virtual void closeEvent(QCloseEvent *e);

protected:
	/// Collection of scan editors
	QStandardItemModel *scanEditorsModel_;
	/// Root item for the scan editors from the app controller
	QStandardItem *scanEditorsParentItem_;
	/// The app controller we're using
	AMDatamanAppController *appController_;
	/// The scan editor that was clicked on to get the context menu
	AMGenericScanEditor *originator_;

	/// Selection view for the scan editors
	QTreeView *treeView_;

	/// Button to select all editors
	QPushButton *selectAllButton_;
	/// Button to select all editor except for the one that was right clicked on
	QPushButton *selectAllOthersButton_;
	/// Button to select all editors that can be closed without saving
	QPushButton *selectSavedButton_;
	/// Button to select all editors with mutltiple scans open
	QPushButton *selectMultiplesButton_;
	/// Button to select all editor with only one scan open
	QPushButton *selectSinglesButton_;

	/// Button to confirm and close selection
	QPushButton *closeButton_;
	/// Button to cancel and close window
	QPushButton *cancelButton_;
};

#endif // AMSCANEDITORSCLOSEVIEW_H
