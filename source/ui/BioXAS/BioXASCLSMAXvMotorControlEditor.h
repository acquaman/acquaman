#ifndef BIOXASCLSMAXVMOTORCONTROLEDITOR_H
#define BIOXASCLSMAXVMOTORCONTROLEDITOR_H

#include <QLabel>

#include "beamline/AMControl.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASCLSMAXvMotorControlEditor : public AMExtendedControlEditor
{
	Q_OBJECT

public:
	BioXASCLSMAXvMotorControlEditor(CLSMAXvMotor* control, AMControl* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);
	~BioXASCLSMAXvMotorControlEditor();

protected slots:
	void onShowContextMenu(const QPoint&);
	void onLimitChanged(CLSMAXvMotor::Limit limit);

protected:
	QLabel *cwLabel_;
	QLabel *ccwLabel_;

};

#endif // BIOXASCLSMAXVMOTORCONTROLEDITOR_H
