#include "BioXASSollerSlitView.h"
#include "beamline/BioXAS/BioXASSollerSlit.h"
#include "beamline/CLS/CLSMAXvMotor.h"

BioXASSollerSlitView::BioXASSollerSlitView(BioXASSollerSlit *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	sollerSlit_ = 0;

	// Create UI elements.

	xMotorEditor_ = new BioXASControlEditor(0);
	xMotorEditor_->setTitle("X motor");
	xMotorEditor_->setMinimumWidth(200);

	zMotorEditor_ = new BioXASControlEditor(0);
	zMotorEditor_->setTitle("Z motor");
	zMotorEditor_->setMinimumWidth(200);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(xMotorEditor_);
	layout->addWidget(zMotorEditor_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASSollerSlitView::~BioXASSollerSlitView()
{

}

void BioXASSollerSlitView::refresh()
{
	updateXMotorEditor();
	updateZMotorEditor();
}

void BioXASSollerSlitView::setControl(BioXASSollerSlit *newControl)
{
	if (sollerSlit_ != newControl) {

		if (sollerSlit_)
			disconnect( sollerSlit_, 0, this, 0 );

		sollerSlit_ = newControl;

		if (sollerSlit_) {
			connect( sollerSlit_, SIGNAL(xMotorChanged(CLSMAXvMotor*)), this, SLOT(updateXMotorEditor()) );
			connect( sollerSlit_, SIGNAL(zMotorChanged(CLSMAXvMotor*)), this, SLOT(updateZMotorEditor()) );
		}

		refresh();

		emit controlChanged(sollerSlit_);
	}
}

void BioXASSollerSlitView::updateXMotorEditor()
{
	if (sollerSlit_)
		xMotorEditor_->setControl(sollerSlit_->xMotor());
	else
		xMotorEditor_->setControl(0);
}

void BioXASSollerSlitView::updateZMotorEditor()
{
	if (sollerSlit_)
		zMotorEditor_->setControl(sollerSlit_->zMotor());
	else
		zMotorEditor_->setControl(0);
}
