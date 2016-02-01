#ifndef BIOXASENDSTATIONTABLEVIEW_H
#define BIOXASENDSTATIONTABLEVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASEndstationTable.h"

class AMExtendedControlEditor;

class BioXASEndstationTableView : public QWidget
{
    Q_OBJECT
public:
	/// constructor
	explicit BioXASEndstationTableView(BioXASEndstationTable *endstationTable, QWidget *parent = 0);

signals:

public slots:

private:
	AMExtendedControlEditor * createControlEditorView(AMControl* control);

protected:
	/// the instance of the enstation table
	BioXASEndstationTable *endstationTable_;

	/// the editor of the height control
	AMExtendedControlEditor *heightControlEditor_;
	/// the editor of the pitch control
	AMExtendedControlEditor *pitchControlEditor_;
	/// the editor of the lateral control
	AMExtendedControlEditor *lateralControlEditor_;
	/// the editor of the yaw control
	AMExtendedControlEditor *yawControlEditor_;
	/// the editor of the roll control
	AMExtendedControlEditor *rollControlEditor_;


};

#endif // BIOXASENDSTATIONTABLEVIEW_H
