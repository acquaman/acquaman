#ifndef REIXSXESSCANACTIONEDITOR_H
#define REIXSXESSCANACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/REIXS/REIXSXESScanActionInfo.h"

class QDoubleSpinBox;

class REIXSXESScanActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor: Must be invokable, and accept a REIXSXESScanActionInfo as argument
	Q_INVOKABLE REIXSXESScanActionEditor(REIXSXESScanActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When the detector energy setpoint spin box is changed
	void onEnergyEditingFinished();
	/// When the focus offset spin box is changed
	void onFocusOffsetEditingFinished();
	/// When the tilt offset spin box is changed
	void onTiltOffsetEditingFinished();
	/// When the "More..." button is clicked, opens up a REIXSXESScanConfigurationView in a dialog window.
	void onMoreDetailsButtonClicked();

	/// Updates the elements when the scan configuration is changed from outside.
	void onScanConfigurationChanged();

protected:
	QDoubleSpinBox* energy_, *focusOffset_, *tiltOffset_;
	REIXSXESScanActionInfo* info_;

};


#endif // REIXSXESSCANACTIONEDITOR_H
