#ifndef AMCHANNELEDITOR_H
#define AMCHANNELEDITOR_H

#include <QWidget>
#include <QMenu>
#include "dataman/AMScanSetModel.h"
#include "ui_AMChannelEditor.h"


/// This class is used inside AMGenericScanEditor to let users create, delete, and modify channel expressions for a set of scans.  The existing channels are displayed in a TreeView list on the left, and the currently-selected channel can be edited in the area to the right.
class AMChannelEditor : public QWidget
{
Q_OBJECT
public:
	explicit AMChannelEditor(AMScanSetModel* model, QWidget *parent = 0);



signals:

public slots:
	/// Call this to set the current scan that we edit channels for
	void setCurrentScan(AMScan* scan);
	/// Call this to set the current scan that we edit channels for. (The index refers to the scan index in our AMScanSetModel)
	void setCurrentScan(int scanIndex);

protected slots:
	/// Called when the currently-selected scan or channel (in our list view) is changed
	void onSetViewIndexChanged(const QModelIndex& selected, const QModelIndex& deselected);
	/// Called when a user clicks the 'close' button on a channel.
	void onCloseButtonClicked(const QModelIndex& index);

	/// Called when the 'add channel' button is clicked
	void onAddChannelButtonClicked();

	/// Called when the user comes up with a name for their new channel
	void onNewChannelNamed();




	// Signals from GUI components to edit the current values
	void expressionEditingFinished(int reason);
	void expressionEditingChanged();
	void descriptionEditingFinished();

	/// Called when a menu item in the raw data menu is clicked:
	void onRawDataMenuTriggered(QAction* action);
	/// Called when a menu item in the function menu is clicked:
	void onFunctionMenuTriggered(QAction* action);





protected:
	// UI components
	Ui::AMChannelEditor ui_;

	/// Menu containing the insert items for the expression editor
	QMenu* insertMenu_, *rawDataMenu_, *functionMenu_;



	/// This is a model of scans and their channels. Inside an AMGenericScanEditor, we share this between ourselves, the main editor, and the AMScanView.
	AMScanSetModel* model_;

	/// Helper function: returns the currently selected scan index.  If a channel is selected, this is the index of its parent scan. If a scan is selected, this is the index of that scan.  If nothing is selected, returns -1;
	int currentScanIndex() const;
	/// Helper function: returns the currently selected channel index.  If a scan (or nothing) is currently selected, returns -1;
	int currentChannelIndex() const;

	/// Helper function: fills the available options in the 'insert' menu based on the selected scan index. Use \c scanIndex = -1 to clear.
	void populateExpressionMenu(int scanIndex);
	/// Helper function: called once to fill the list of functions.  This should really be tied into the muParser stuff, but it's here for now.
	void populateFunctionMenu();

	/// You can end channel expression editing in two ways: by accepting it (if its valid), or...
	/*! This function is designed to do nothing if a channel expression edit is not in progress.*/
	void finalizeExpressionEdit();
	/// You can end channel expression editing by aborting it completely:
	/*! This function is designed to do nothing if a channel expression edit is not in progress.*/
	void abortExpressionEdit();



	/// Flag to indicate that we're currently editing the name of a new (not-yet created) channel
	bool editingNewChannel_;
	/// Flag to indicate that we're currently editing the expression for a valid channel. So be careful if you switch channels suddenly!
	bool editingExpression_;


};

#endif // AMCHANNELEDITOR_H
