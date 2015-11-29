#include "SGMSampleChamberVacuumMoveView.h"
#include "beamline/SGM/SGMSampleChamberVacuumMoveControl.h"
#include "ui/SGM/SGMVATValveView.h"

SGMSampleChamberVacuumMoveView::SGMSampleChamberVacuumMoveView(SGMSampleChamberVacuumMoveControl *move, QWidget *parent) : QWidget(parent)
{
	// Initialize class variables.

	move_ = 0;

	// Create UI elements.

	// Current settings.

	setMove(move);

	refresh();
}

SGMSampleChamberVacuumMoveView::~SGMSampleChamberVacuumMoveView()
{

}

void SGMSampleChamberVacuumMoveView::refresh()
{

}

void SGMSampleChamberVacuumMoveView::setMove(SGMSampleChamberVacuumMoveControl *newControl)
{
	if (move_ != newControl) {

		if (move_)
			disconnect( move_, 0, this, 0 );

		move_ = newControl;

		if (move_) {
			connect( move_, SIGNAL(pressureChanged(AMControl*)), this, SLOT(updatePressureEditor()) );
			connect( move_, SIGNAL(vatValveChanged(SGMVATValve*)), this, SLOT(updateVATValveView()) );
			connect( move_, SIGNAL(turbosChanged(AMControlSet*)), this, SLOT(refreshTurbosView()) );
		}

		refresh();

		emit moveChanged(move_);
	}
}

void SGMSampleChamberVacuumMoveView::clear()
{

}
