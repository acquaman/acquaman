#ifndef SGMSAMPLECHAMBERVACUUMEDITOR_H
#define SGMSAMPLECHAMBERVACUUMEDITOR_H

#include "ui/beamline/AMExtendedControlEditor.h"

class SGMSampleChamberVacuum;

class SGMSampleChamberVacuumEditor : public AMExtendedControlEditor
{
	Q_OBJECT

public:
	/// Constructor.
	SGMSampleChamberVacuumEditor(SGMSampleChamberVacuum *vacuum, QWidget *parent = 0);
	/// Destructor.
	virtual ~SGMSampleChamberVacuumEditor();

public slots:
	/// Sets the vacuum control. Reimplemented to listen to when the control starts moving, so it can show the move view.
	virtual void setControl(SGMSampleChamberVacuum *newControl);

protected slots:
	void onVacuumMoveStarted();
};

#endif // SGMSAMPLECHAMBERVACUUMEDITOR_H
