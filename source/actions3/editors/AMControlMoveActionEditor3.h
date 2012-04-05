#ifndef AMCONTROLMOVEACTIONEDITOR3_H
#define AMCONTROLMOVEACTIONEDITOR3_H

#include <QFrame>
#include "actions3/actions/AMControlMoveActionInfo3.h"

class QDoubleSpinBox;
class QComboBox;
class QLabel;
class QCheckBox;

/// This widget provides an in-place editor for AMControlMoveActionInfo.
class AMControlMoveActionEditor3 : public QFrame
{
    Q_OBJECT
public:
	/// Constructor: Must be invokable, and accept a AMControlMoveActionInfo as argument
	Q_INVOKABLE AMControlMoveActionEditor3(AMControlMoveActionInfo3 *info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the combo box is activated to choose a different control. Sets the min/max range of the setpointBox_, and sets the info_ to use that control.
	void onControlSelectorBoxCurrentIndexChanged(int index);
	/// Called when the user is done editing the setpointBox_ target value.
	void onSetpointBoxEditingFinished();
	/// Called when the "relative move" checkbox is toggled
	void onRelativeCheckBoxToggled(bool isRelative);

protected:
	AMControlMoveActionInfo3 *info_;

	/// Holds the descriptions (as DisplayRole text) and names (as UserRole) of the controls we have available to choose from.
	QComboBox* controlSelectorBox_;
	QDoubleSpinBox* setpointBox_;
	QLabel* unitsLabel_, *toLabel_;
	QCheckBox* relativeCheckBox_;

	/// Helper function to populate the available controls in the controlSelectorBox_, and update the widgets to reflect the current info_.
	void populateControls();
};

#endif // REIXSCONTROLMOVEACTIONEDITOR_H
