#ifndef MAXVMOTORCONTROLEDITOR_H
#define MAXVMOTORCONTROLEDITOR_H

#include "ui/beamline/AMExtendedControlEditor.h"
#include "../BioXASMAXvMotor.h"

class MAXvMotorControlEditor : public AMExtendedControlEditor
{
public:
	MAXvMotorControlEditor(BioXASMAXvMotor* control, BioXASMAXvMotor* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);
};

#endif // MAXVMOTORCONTROLEDITOR_H
