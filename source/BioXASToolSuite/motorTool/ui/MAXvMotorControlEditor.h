#ifndef MAXVMOTORCONTROLEDITOR_H
#define MAXVMOTORCONTROLEDITOR_H

#include "ui/beamline/AMExtendedControlEditor.h"
#include "beamline/AMControl.h"

#include "../BioXASMAXvMotor.h"

class MAXvMotorControlEditor : public AMExtendedControlEditor
{
public:
		MAXvMotorControlEditor(BioXASMAXvMotor* control, AMControl* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);
};

#endif // MAXVMOTORCONTROLEDITOR_H
