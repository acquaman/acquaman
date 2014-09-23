#ifndef MAXVMOTORCONTROLEDITOR_H
#define MAXVMOTORCONTROLEDITOR_H

#include <QLabel>

#include "beamline/AMControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include "../BioXASMAXvMotor.h"

class MAXvMotorControlEditor : public AMExtendedControlEditor
{
public:
	MAXvMotorControlEditor(BioXASMAXvMotor* control, AMControl* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);

protected slots:
	void onLimitChanged(CLSMAXvMotor::Limit limit);

protected:
	QLabel *cwLabel_;
	QLabel *ccwLabel_;
};

#endif // MAXVMOTORCONTROLEDITOR_H
