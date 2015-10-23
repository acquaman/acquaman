#include "SGMVelocityTests.h"

#include "beamline/AMPVControl.h"
#include <QDebug>
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMControlSet.h"
VelocityTestConditions::VelocityTestConditions(double encStart, double encEnd, double gratingTransTarget, double velo, double veloBase, double accel)
{
	encoderStart = encStart;
	encoderEnd = encEnd;
	gratingTranslationTarget = gratingTransTarget;
	velocity = velo;
	velocityBase = veloBase;
	acceleration = accel;
}

SGMVelocityTests::SGMVelocityTests(QObject* parent) :
	QObject(parent)
{
	gratingAngleVelocityFeedback_ = new AMSinglePVControl("Grating angle velocity feedback",
														  "SMTR16114I1002:velo:fbk",
														  this);

	gratingAngleEncoderFeedback_ = new AMSinglePVControl("Grating encoder feedback",
													"SMTR16114I1002:enc:fbk",
													this,
													10);

	gratingAngleVelocity_ = new AMSinglePVControl("Grating angle velocity setpoint",
												  "SMTR16114I1002:velo",
												  this);

	gratingAngleVelocityBase_ = new AMSinglePVControl("Grating angle velocity base",
													  "SMTR16114I1002:veloBase",
													  this);

	gratingAngleAcceleration_ = new AMSinglePVControl("Grating angle acceleration",
													  "SMTR16114I1002:accel",
													  this);

	gratingAngleEncoderSetpoint_ = new AMSinglePVControl("Grating angle step setpoint",
														 "SMTR16114I1002:encTarget",
														 this);

	gratingTranslationSetpoint_ = new AMSinglePVControl("Grating translation setpoint",
														"SMTR16114I1016:step",
														this);

	gratingTranslationFeedback_ = new AMSinglePVControl("Grating translation feedback",
														"SMTR16114I1016:step:fbk",
														this,
														2);

	allControls_ = new AMControlSet(this);
	connectedOnce_ = false;
	allControls_->addControl(gratingAngleVelocityFeedback_);
	allControls_->addControl(gratingAngleEncoderFeedback_);
	allControls_->addControl(gratingAngleVelocity_);
	allControls_->addControl(gratingAngleVelocityBase_);
	allControls_->addControl(gratingAngleAcceleration_);
	allControls_->addControl(gratingAngleEncoderSetpoint_);
	allControls_->addControl(gratingTranslationSetpoint_);
	allControls_->addControl(gratingTranslationFeedback_);

	if(allControls_->isConnected()) {

		onAllControlsConnected(true);
	} else {

		connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
	}

	timer_.setInterval(500);
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

	currentTestAction_ = 0;
}

void SGMVelocityTests::testGratingAngle()
{
	// From 270ev (-412450) to 320eV (-348005)
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 5000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 5000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 5000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 5000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 5000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 5000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 2500));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 2500));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 2500));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 2500));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 2500));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 2500));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 1000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 1000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 500, 1000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 500, 1000));

	// From 270ev (-412450) to 320eV (-348005)
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 5000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 5000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 5000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 5000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 5000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 5000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 2500));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 2500));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 2500));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 2500));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 2500));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 2500));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 1000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 0, 1000));

	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 500, 1000));
	testQueue_.push_back(VelocityTestConditions(-412450, -348005, 61300, 12000, 500, 1000));

	// From 690eV (-295012) to 750eV (-271411)
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 0, 5000));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 0, 5000));

	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 5000));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 5000));

	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 5000));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 5000));

	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 0, 2500));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 0, 2500));

	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 2500));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 2500));

	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 2500));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 2500));

	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 0, 1000));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 0, 1000));

	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 500, 1000));
	testQueue_.push_back(VelocityTestConditions(-295015, -271411, 35100, 12000, 500, 1000));

	runNextTest();
}

void SGMVelocityTests::onTimerTimeout()
{
	qDebug() << interval_ << ","
			 << gratingAngleVelocityFeedback_->value() << ","
			 << gratingAngleEncoderFeedback_->value();

	interval_ += 500;
}

void SGMVelocityTests::onAllControlsConnected(bool allConnected)
{
	if(allConnected && !connectedOnce_) {

		connectedOnce_ = true;
		testGratingAngle();
	}
}

void SGMVelocityTests::runNextTest()
{
	if(!currentTestAction_) {

		// Ensure is not the first test before attempting to delete the previous
		currentTestAction_->deleteLater();
	}

	timer_.stop();
	interval_ = 500;

	if(!testQueue_.isEmpty()) {
		VelocityTestConditions nextTest = testQueue_.first();
		testQueue_.removeFirst();
		buildTest(nextTest);

		// Insert a delay of 20 ms to stop EPICS from crying
		epicsDelay_.singleShot(20, this, SLOT(epicsDelayElapsed()));
	}
}

void SGMVelocityTests::epicsDelayElapsed()
{
	currentTestAction_->start();
	timer_.start();
	onTimerTimeout();
}

void SGMVelocityTests::buildTest(const VelocityTestConditions& testConditions)
{
	qDebug() << "\n\nStart:" << testConditions.encoderStart
			 << ",End:" << testConditions.encoderEnd
			 << ",Grating:" << testConditions.gratingTranslationTarget
			 << ",Velocity:" << testConditions.velocity
			 << ",Velocity Base:" << testConditions.velocityBase
			 <<	",Acceleration:"<< testConditions.acceleration;

	currentTestAction_ = new AMListAction3(new AMListActionInfo3("Test", "Test"),
														 AMListAction3::Sequential);

	connect(currentTestAction_, SIGNAL(succeeded()), this, SLOT(runNextTest()));

	// Setup initial conditions
	AMListAction3* setInitialConditionsAction = new AMListAction3(new AMListActionInfo3("Init", "Init"),
																  AMListAction3::Parallel);


	setInitialConditionsAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleVelocity_, testConditions.velocity));
	setInitialConditionsAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleVelocityBase_, testConditions.velocityBase));
	setInitialConditionsAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleAcceleration_, testConditions.acceleration));
	setInitialConditionsAction->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslationSetpoint_, testConditions.gratingTranslationTarget));

	currentTestAction_->addSubAction(setInitialConditionsAction);

	// Wait for initial conditions
	AMListAction3* waitForInitialConditionsAction = new AMListAction3(new AMListActionInfo3("Init Wait", "Init Wait"),
																	 AMListAction3::Parallel);

	waitForInitialConditionsAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleVelocity_, testConditions.velocity));
	waitForInitialConditionsAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleVelocityBase_, testConditions.velocityBase));
	waitForInitialConditionsAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleAcceleration_, testConditions.acceleration));
	waitForInitialConditionsAction->addSubAction(AMActionSupport::buildControlWaitAction(gratingTranslationSetpoint_, testConditions.gratingTranslationTarget));

	currentTestAction_->addSubAction(waitForInitialConditionsAction);

	// Move grating angle to its starting position
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleEncoderSetpoint_, testConditions.encoderStart));

	// Wait for grating angle to reach its starting point
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleEncoderFeedback_, testConditions.encoderStart));

	// Wait for grating translation to reach its target
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingTranslationFeedback_, testConditions.gratingTranslationTarget));

	// Move to final position and start up the timer.
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleEncoderSetpoint_, testConditions.encoderEnd));

	// Wait for grating angle to reach its final destination
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleEncoderFeedback_, testConditions.encoderEnd));
}



