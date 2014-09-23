#include "MAXvMotorControlEditor.h"

#include <QDebug>
#include <QLabel>

MAXvMotorControlEditor::MAXvMotorControlEditor(BioXASMAXvMotor* control, AMControl* statusTagControl, bool readOnly, bool configureOnly, QWidget *parent)
	:AMExtendedControlEditor(control, statusTagControl, readOnly, configureOnly, parent)
{
	valueLabel_->setToolTip(control->feedbackPVName());
	unitsLabel_->setToolTip(control->feedbackPVName() + " (units)");
	statusLabel_->setToolTip(control->statusPVName());
}
