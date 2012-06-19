#ifndef AMSAMPLEPLATEMOVEACTIONEDITOR_H
#define AMSAMPLEPLATEMOVEACTIONEDITOR_H

#include <QFrame>
#include "actions3/actions/AMSamplePlateMoveActionInfo.h"

class QComboBox;
class QLabel;
class QCheckBox;

#define AMSAMPLEPLATEMOVEACTIONEDITOR_ATTEMPTED_TO_LOAD_BAD_PLATE 327201

/// This widget provides an in-place editor for AMControlMoveActionInfo.
class AMSamplePlateMoveActionEditor : public QFrame
{
Q_OBJECT

public:
	/// Constructor
	Q_INVOKABLE AMSamplePlateMoveActionEditor(AMSamplePlateMoveActionInfo *info, QWidget *parent = 0);

protected slots:
	/// Called when the sample position combo box is activated to choose a different sample on the plate.
	void onSamplePositionSelectorBoxCurrentIndexChanged(int index);
	/// Called when the sample plate combo box is activated to choose a different sample plate altogether
	void onSamplePlateSeletectorBoxCurrentIndexChanged(int index);
	/// Called when the change sample plate check is toggled
	void onChangeSamplePlateCheckBoxToggled(bool canChangePlate);


protected:
	AMSamplePlateMoveActionInfo *info_;

	/// Holds the descriptions (as DisplayRole text) and names (as UserRole) of the controls we have available to choose from.
	QComboBox* samplePositionSelectorBox_;
	QComboBox* samplePlateSelectorBox_;
	QLabel* unitsLabel_, *toLabel_;
	QCheckBox* changeSamplePlateCheckBox_;

	/// Helper function to populate the available sample plates, and update the widgets to reflect the current info_.
	void populateSamplePlates();

	/// Helper function to populate the available sample positions, and update the widgets to reflect the current info_.
	void populateSamplePositions();
};

#endif // AMSAMPLEPLATEMOVEACTIONEDITOR_H
