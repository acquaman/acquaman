#include "SGMSampleChamberVacuumEditor.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"

SGMSampleChamberVacuumEditor::SGMSampleChamberVacuumEditor(SGMSampleChamberVacuum *vacuum, QWidget *parent) :
	AMExtendedControlEditor(vacuum, 0, false, false, parent)
{
	setNoUnitsBox(true);
}

SGMSampleChamberVacuumEditor::~SGMSampleChamberVacuumEditor()
{

}

void SGMSampleChamberVacuumEditor::setControl(SGMSampleChamberVacuum *newControl)
{
	if (control_)
		disconnect( control_, 0, this, 0 );

	control_ = newControl;

	if (control_)
		connect( control_, SIGNAL(moveStarted()), this, SLOT(onVacuumMoveStarted()) );
}

void SGMSampleChamberVacuumEditor::onVacuumMoveStarted()
{

}

