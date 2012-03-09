#ifndef REIXSBEAMONOFFACTIONEDITOR_H
#define REIXSBEAMONOFFACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/REIXS/REIXSBeamOnOffActionInfo.h"

/// Provides an in-place editor for REIXS Beam on/off actions: lets you choose beam on or off.
class REIXSBeamOnOffActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor: Must be Q_INVOKABLE, and accept an REIXSBeamOnOffActionInfo as argument
	Q_INVOKABLE REIXSBeamOnOffActionEditor(REIXSBeamOnOffActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the beam on radio button is toggled, to set the info_.
	void onBeamOnRadioButtonToggled(bool beamOn);

protected:
	REIXSBeamOnOffActionInfo* info_;

};

#endif // REIXSBEAMONOFFACTIONEDITOR_H
