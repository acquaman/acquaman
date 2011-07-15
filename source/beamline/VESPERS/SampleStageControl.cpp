#include "SampleStageControl.h"

SampleStageControl::SampleStageControl(AMControl *horiz, AMControl *vert, AMControl *norm, QObject *parent)
	: QObject(parent)
{
	// The limits.
	xRange_ = qMakePair(0, 0);
	yRange_ = qMakePair(0, 0);
	zRange_ = qMakePair(0, 0);

	// The motor controls
	horiz_ = horiz;
	vert_ = vert;
	norm_ = norm;

	connect(horiz_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(vert_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(norm_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	connect(horiz_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(vert_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(norm_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));

	connect(horiz_, SIGNAL(setpointChanged(double)), this, SIGNAL(horizontalSetpointChanged(double)));
	connect(vert_, SIGNAL(setpointChanged(double)), this, SIGNAL(verticalSetpointChanged(double)));
	connect(norm_, SIGNAL(setpointChanged(double)), this, SIGNAL(normalSetpointChanged(double)));

	// If the controls are CLS VME motors, then use their connections to their steps.  Otherwise, it is a pseudomotor and we'll need to create those connections ourselves.
	CLSVMEMotor *x = qobject_cast<CLSVMEMotor *>(horiz_);
	CLSVMEMotor *y = qobject_cast<CLSVMEMotor *>(vert_);
	CLSVMEMotor *z = qobject_cast<CLSVMEMotor *>(norm_);

	if (x == 0 && y == 0 && z == 0){

		connect(x, SIGNAL(stepChanged(double)), this, SLOT(onXStepChanged(double)));
		connect(y, SIGNAL(stepChanged(double)), this, SLOT(onYStepChanged(double)));
		connect(z, SIGNAL(stepChanged(double)), this, SLOT(onZStepChanged(double)));
	}
	else{

		AMProcessVariable *xStep = new AMProcessVariable("SVM1607-2-B21-02:step:sp", true, this);
		AMProcessVariable *yStep = new AMProcessVariable("SVM1607-2-B21-03:step:sp", true, this);
		AMProcessVariable *zStep = new AMProcessVariable("SVM1607-2-B21-01:step:sp", true, this);

		connect(xStep, SIGNAL(valueChanged(double)), this, SLOT(onXStepChanged(double)));
		connect(yStep, SIGNAL(valueChanged(double)), this, SLOT(onYStepChanged(double)));
		connect(zStep, SIGNAL(valueChanged(double)), this, SLOT(onZStepChanged(double)));
	}
}

void SampleStageControl::onXStepChanged(double step)
{
	if (step > xRange_.first && step < xRange_.second){

		stopAll();
		emit moveError("X-direction motor at limit.");
	}
}

void SampleStageControl::onYStepChanged(double step)
{
	if (step > yRange_.first && step < yRange_.second){

		stopAll();
		emit moveError("Y-direction motor at limit.");
	}
}

void SampleStageControl::onZStepChanged(double step)
{
	if (step > zRange_.first && step < zRange_.second){

		stopAll();
		emit moveError("Z-direction motor at limit.");
	}
}
