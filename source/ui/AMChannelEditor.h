#ifndef AMCHANNELEDITOR_H
#define AMCHANNELEDITOR_H

#include <QWidget>
#include "dataman/AMScanSetModel.h"

/// This class is used inside AMGenericScanEditor to let users create, delete, and modify channel expressions for a set of scans.  The existing channels are displayed in a TreeView list on the left, and the currently-selected channel can be edited in the area to the right.
class AMChannelEditor : public QWidget
{
Q_OBJECT
public:
	explicit AMChannelEditor(AMScanSetModel* model, QWidget *parent = 0);

	void setCurrentScan(AMScan* scan);
	void setCurrentScan(int scanIndex);

signals:

public slots:

};

#endif // AMCHANNELEDITOR_H
