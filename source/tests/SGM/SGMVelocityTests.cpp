#include "SGMVelocityTests.h"

#include "beamline/AMPVControl.h"
#include <QDebug>
SGMVelocityTests::SGMVelocityTests(QObject* parent) :
	QObject(parent)
{
}

void SGMVelocityTests::testGratingAngle()
{

	AMSinglePVControl* gratingAngleVelocityFeedback =
			new AMSinglePVControl("Grating angle velocity feedback",
								  "SMTR16114I1002:velo:fbk",
								  this);


	AMSinglePVControl* gratingAngleStepSetpoint =
			new AMSinglePVControl("Grating angle step setpoint",
								  "SMTR16114I1002:encTarget",
								  this);


	connect(gratingAngleVelocityFeedback, SIGNAL(valueChanged(double)),
			this, SLOT(onGratingAngleVelocityFeedbackChanged(double)));

	timer_.restart();
	gratingAngleStepSetpoint->move(-271413);

}

void SGMVelocityTests::onGratingAngleVelocityFeedbackChanged(double value)
{
	qDebug() << timer_.elapsed() << "," << value;
}
