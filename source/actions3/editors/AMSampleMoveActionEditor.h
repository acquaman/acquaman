#ifndef AMSAMPLEMOVEACTIONEDITOR_H
#define AMSAMPLEMOVEACTIONEDITOR_H

#include <QFrame>

#include "actions3/actions/AMSampleMoveActionInfo.h"

class QComboBox;
class QLabel;

class AMSampleMoveActionEditor : public QFrame
{
Q_OBJECT
public:
	Q_INVOKABLE AMSampleMoveActionEditor(AMSampleMoveActionInfo *info, QWidget *parent = 0);

	virtual ~AMSampleMoveActionEditor();

protected slots:
	/// Called when the sample plate combo box is activated to choose a different sample plate altogether
	void onSampleSeletectorBoxCurrentIndexChanged(int index);

protected:
	AMSampleMoveActionInfo *info_;

	/// Holds the descriptions (as DisplayRole text) and names (as UserRole) of the controls we have available to choose from.
	QComboBox* sampleSelectorBox_;
	QLabel* unitsLabel_, *toLabel_;

	/// Helper function to populate the available sample positions, and update the widgets to reflect the current info_.
	void populateSamples();
};

#endif // AMSAMPLEMOVEACTIONEDITOR_H
