#ifndef AMCHANNELEDITOR_H
#define AMCHANNELEDITOR_H

#include <QWidget>
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





protected:
	Ui::AMChannelEditor ui_;

	AMScanSetModel* model_;

	/// Helper function: returns the currently selected scan index.  If a channel is selected, this is the index of its scan. If a scan is selected, this is the index of that scan.  If nothing is selected, returns -1;
	int currentScanIndex() const;


};

#endif // AMCHANNELEDITOR_H
