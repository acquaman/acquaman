#ifndef BIOXASPSEUDOMOTORCONTROLEDITOR_H
#define BIOXASPSEUDOMOTORCONTROLEDITOR_H

#include <QLabel>

#include "beamline/AMControl.h"
#include "beamline/BioXAS/BioXASPseudoMotorControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASPseudoMotorControlEditor : public AMExtendedControlEditor
{
	Q_OBJECT

public:
	BioXASPseudoMotorControlEditor(BioXASPseudoMotorControl* control, AMControl* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);
	~BioXASPseudoMotorControlEditor() {}

protected slots:
	/// the slot to handle the context menu
	void onShowContextMenu(const QPoint&);
	/// the slot to handle the enabledPVStatusChanged signal
	void onEnabledPVStatusChanged(double);

protected:
	QLabel *enabledStatusLabel_;
};

#endif // BIOXASPSEUDOMOTORCONTROLEDITOR_H
