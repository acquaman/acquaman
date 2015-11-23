#include "SGMGratingVelocityTests.h"

#include "beamline/AMPVControl.h"
#include <QDebug>
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMControlSet.h"
#include "actions3/actions/AMWaitAction.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/SGM/energy/SGMGratingTranslationStepControl.h"
GratingVelocityTestConditions::GratingVelocityTestConditions(double encStart, double encEnd, double gratingTransTarget, double velo, double veloBase, double accel)
{
	encoderStart = encStart;
	encoderEnd = encEnd;
	gratingTranslationTarget = gratingTransTarget;
	velocity = velo;
	velocityBase = veloBase;
	acceleration = accel;
}

SGMGratingVelocityTests::SGMGratingVelocityTests(QObject* parent) :
	QObject(parent)
{
    gratingAngleEncoder_ =  new AMPVwStatusControl("Grating Angle",
                                                   "SMTR16114I1002:enc:fbk",
                                                   "SMTR16114I1002:encTarget",
                                                   "SMTR16114I1002:status",
                                                   "SMTR16114I1002:stop",
                                                   this,
                                                   5,
                                                   2.0,
                                                   new CLSMAXvControlStatusChecker(),
                                                   1);

    gratingTranslation_ = new SGMGratingTranslationStepControl(this);

    gratingAngleVelocitySetpoint_ = new AMSinglePVControl("Grating Angle Velocity Setpoint",
                                                          "SMTR16114I1002:velo",
                                                          this,
                                                          0.1);

    gratingAngleVelocityFeedback_ = new AMSinglePVControl("Grating angle velocity feedback",
                                                          "SMTR16114I1002:velo:fbk",
                                                          this,
                                                          0.1);

    gratingAngleVelocityBase_ = new AMSinglePVControl("Grating angle velocity base",
                                                      "SMTR16114I1002:veloBase",
                                                      this,
                                                      0.1);

    gratingAngleAcceleration_ = new AMSinglePVControl("Grating angle acceleration",
                                                      "SMTR16114I1002:accel",
                                                      this,
                                                      0.1);

	allControls_ = new AMControlSet(this);
	connectedOnce_ = false;


	allControls_->addControl(gratingAngleEncoder_);
	allControls_->addControl(gratingTranslation_);
	allControls_->addControl(gratingAngleVelocityBase_);
	allControls_->addControl(gratingAngleAcceleration_);

	if(allControls_->isConnected()) {

		onAllControlsConnected(true);
	} else {

		connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
	}

	timer_.setInterval(500);
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

	currentTestAction_ = 0;
}

void SGMGratingVelocityTests::testGratingAngle()
{
	// From 270ev (-412450) to 320eV (-348005)
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -310000, 61300, 6000, 0, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -313205, 61300, 3276, 0, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 0, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 0, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 1000, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 3000, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 0, 1000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 0, 1000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 500, 1000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -348005, 61300, 12000, 500, 1000));

	// From 270ev (-412450) to 320eV (-348005)
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 0, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 0, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 1000, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 1000, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 3000, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 3000, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 0, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 0, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 1000, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 1000, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 3000, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 3000, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 0, 1000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 0, 1000));

	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 500, 1000));
	testQueue_.push_back(GratingVelocityTestConditions(-412450, -397720, 61300, 12000, 500, 1000));

	// From 690eV (-295012) to 750eV (-271411)
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 0, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 0, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 5000));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 5000));

	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 0, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 0, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 1000, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 2500));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 3000, 2500));

	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 0, 1000));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 0, 1000));

	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 500, 1000));
	testQueue_.push_back(GratingVelocityTestConditions(-295015, -271411, 35100, 12000, 500, 1000));

	runNextTest();
}

void SGMGratingVelocityTests::onTimerTimeout()
{
	qDebug() << interval_ << ","
			 << gratingAngleVelocityFeedback_->value() << ","
			 << gratingAngleEncoder_->value();

	interval_ += 500;
}

void SGMGratingVelocityTests::onAllControlsConnected(bool allConnected)
{
	if(allConnected && !connectedOnce_) {

		connectedOnce_ = true;
		testGratingAngle();
	}
}

void SGMGratingVelocityTests::runNextTest()
{
	if(currentTestAction_) {

		// Ensure is not the first test before attempting to delete the previous
		currentTestAction_->deleteLater();
	}

	timer_.stop();
	interval_ = 500;

	if(!testQueue_.isEmpty()) {
		GratingVelocityTestConditions nextTest = testQueue_.first();
		testQueue_.removeFirst();
		buildTest(nextTest);

		// Insert a delay of 20 ms to stop EPICS from crying
		epicsDelay_.singleShot(20, this, SLOT(epicsDelayElapsed()));
	}
}

void SGMGratingVelocityTests::epicsDelayElapsed()
{
	connect(currentTestAction_, SIGNAL(succeeded()), this, SLOT(runNextTest()));
	connect(currentTestAction_, SIGNAL(started()), this, SLOT(onTestStarted()));
	if(!currentTestAction_->start()) {

		qDebug() << "Couldn't start test";
	}
}

void SGMGratingVelocityTests::onTestStarted()
{
	timer_.start();
	onTimerTimeout();
}

void SGMGratingVelocityTests::buildTest(const GratingVelocityTestConditions& testConditions)
{
	qDebug() << "\n\nStart:" << testConditions.encoderStart
			 << ",End:" << testConditions.encoderEnd
			 << ",Grating:" << testConditions.gratingTranslationTarget
			 << ",Velocity:" << testConditions.velocity
			 << ",Velocity Base:" << testConditions.velocityBase
			 <<	",Acceleration:"<< testConditions.acceleration;

	qDebug() << "Time(ms), Velocity, Encoder Feedback";

	currentTestAction_ = new AMListAction3(new AMListActionInfo3("Test", "Test"),
														 AMListAction3::Sequential);

	// Move the grating translation to its required position
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingTranslation_, testConditions.gratingTranslationTarget));

	// Wait for grating translation to reach its target
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingTranslation_, testConditions.gratingTranslationTarget, 60, AMControlWaitActionInfo::MatchWithinTolerance));

	if(!gratingAngleEncoder_->withinTolerance(testConditions.encoderStart)) {

		// Move grating angle to its starting position
		currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleEncoder_, testConditions.encoderStart));

		// Wait for grating angle to reach its starting point
		currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleEncoder_, testConditions.encoderStart, 60, AMControlWaitActionInfo::MatchWithinTolerance));

		currentTestAction_->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.5)));
	}

	// Setup initial conditions
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleVelocitySetpoint_, testConditions.velocity));
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleVelocitySetpoint_, testConditions.velocity, 2, AMControlWaitActionInfo::MatchWithinTolerance));
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleVelocityBase_, testConditions.velocityBase));
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleVelocityBase_, testConditions.velocityBase, 2, AMControlWaitActionInfo::MatchWithinTolerance));
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleAcceleration_, testConditions.acceleration));
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleAcceleration_, testConditions.acceleration, 2, AMControlWaitActionInfo::MatchWithinTolerance));

	// Move to final position.
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(gratingAngleEncoder_, testConditions.encoderEnd));

	// Wait for grating angle to reach its final destination
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(gratingAngleEncoder_, testConditions.encoderEnd, 60, AMControlWaitActionInfo::MatchWithinTolerance));
}




