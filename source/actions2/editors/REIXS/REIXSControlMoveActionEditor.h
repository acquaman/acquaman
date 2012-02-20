#ifndef REIXSCONTROLMOVEACTIONEDITOR_H
#define REIXSCONTROLMOVEACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/REIXS/REIXSControlMoveActionInfo.h"

class QDoubleSpinBox;
class QComboBox;
class QLabel;

/// This widget provides an in-place editor for REIXSControlMoveActionInfo.
class REIXSControlMoveActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor: Must be invokable, and accept a REIXSControlMoveActionInfo as argument
	Q_INVOKABLE REIXSControlMoveActionEditor(REIXSControlMoveActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the combo box is activated to choose a different control. Sets the min/max range of the setpointBox_, and sets the info_ to use that control.
	void onControlSelectorBoxCurrentIndexChanged(int index);
	/// Called when the user is done editing the setpointBox_ target value.
	void onSetpointBoxEditingFinished();

protected:
	REIXSControlMoveActionInfo* info_;

	/// Holds the descriptions (as DisplayRole text) and names (as UserRole) of the controls we have available to choose from.
	QComboBox* controlSelectorBox_;
	QDoubleSpinBox* setpointBox_;
	QLabel* unitsLabel_;

	/// Helper function to populate the available controls in the controlSelectorBox_, and update the widgets to reflect the current info_.
	void populateControls();
};

#endif // REIXSCONTROLMOVEACTIONEDITOR_H
