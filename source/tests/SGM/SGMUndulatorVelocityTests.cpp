#include "SGMUndulatorVelocityTests.h"

#include "beamline/AMPVControl.h"
#include <QDebug>
#include "actions3/AMListAction3.h"
#include "actions3/AMActionSupport.h"
#include "beamline/AMControlSet.h"
#include "actions3/actions/AMWaitAction.h"
#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/SGM/energy/SGMGratingTranslationStepControl.h"

UndulatorVelocityTestConditions::UndulatorVelocityTestConditions(double encStart, double encEnd, double velo, double veloBase, double accel)
{
	encoderStart = encStart;
	encoderEnd = encEnd;
	velocity = velo;
	velocityBase = veloBase;
	acceleration = accel;
}

SGMUndulatorVelocityTests::SGMUndulatorVelocityTests(QObject *parent) :
    QObject(parent)
{
	connectedOnce_ = false;
    undulatorEncoder_ = new AMPVwStatusControl("Undulator encoder step",
                                               "SMTR1411-01:step:sp",		// Yep, seriously. The feedback pv is called :sp
                                               "SMTR1411-01:step",
                                               "SMTR1411-01:status",
                                               QString(),
                                               this,
                                               0.01);

    undulatorVelocitySetpoint_ = new AMSinglePVControl("Undulator velocity setpoint",
                                                       "SMTR1411-01:velo",
                                                       this,
                                                       0.1);

    undulatorVelocityFeedback_ = new AMSinglePVControl("Undulator velocity feedback",
                                                       "SMTR1411-01:velo:fbk",
                                                       this,
                                                       0.1);

    undulatorVelocityBase_ = new AMSinglePVControl("Undulator velocity base",
                                                   "SMTR1411-01:veloBase",
                                                   this,
                                                   0.1);

    undulatorAcceleration_ = new AMSinglePVControl("Undulator acceleration",
                                                   "SMTR1411-01:accel",
                                                   this,
                                                   0.1);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(undulatorEncoder_);
	allControls_->addControl(undulatorVelocitySetpoint_);
	allControls_->addControl(undulatorVelocityFeedback_);
	allControls_->addControl(undulatorVelocityBase_);
	allControls_->addControl(undulatorAcceleration_);

	if(allControls_->isConnected()) {

		onAllControlsConnected(true);
	} else {

		connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));

	}

	timer_.setInterval(500);
	connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));

	currentTestAction_ = 0;
}

void SGMUndulatorVelocityTests::testUndulator()
{
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 0, 5000));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 0, 5000));

	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 1000, 5000));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 1000, 5000));

	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 3000, 5000));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 3000, 5000));

	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 0, 2500));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 0, 2500));

	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 1000, 2500));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 1000, 2500));

	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 3000, 2500));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 3000, 2500));

	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 0, 1000));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 0, 1000));

	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 500, 1000));
	testQueue_.push_back(UndulatorVelocityTestConditions(-148679, -131784, 12000, 500, 1000));

	runNextTest();
}

void SGMUndulatorVelocityTests::onTimerTimeout()
{
	qDebug() << interval_ << ","
			 << undulatorVelocityFeedback_->value() << ","
			 << undulatorEncoder_->value();

	interval_ += 500;
}

void SGMUndulatorVelocityTests::onAllControlsConnected(bool allConnected)
{
	if(allConnected && !connectedOnce_) {

		connectedOnce_ = true;
		testUndulator();
	}
}

void SGMUndulatorVelocityTests::runNextTest()
{
	if(currentTestAction_) {

		// Ensure is not the first test before attempting to delete the previous
		currentTestAction_->deleteLater();
	}

	timer_.stop();
	interval_ = 500;

	if(!testQueue_.isEmpty()) {
		UndulatorVelocityTestConditions nextTest = testQueue_.first();
		testQueue_.removeFirst();
		buildTest(nextTest);

		// Insert a delay of 20 ms to stop EPICS from crying
		epicsDelay_.singleShot(20, this, SLOT(epicsDelayElapsed()));
	}
}

void SGMUndulatorVelocityTests::epicsDelayElapsed()
{
	connect(currentTestAction_, SIGNAL(succeeded()), this, SLOT(runNextTest()));
	connect(currentTestAction_, SIGNAL(started()), this, SLOT(onTestStarted()));
	if(!currentTestAction_->start()) {

		qDebug() << "Couldn't start test";
	}
}

void SGMUndulatorVelocityTests::onTestStarted()
{
	timer_.start();
	onTimerTimeout();
}

void SGMUndulatorVelocityTests::buildTest(const UndulatorVelocityTestConditions &testConditions)
{
	qDebug() << "\n\nStart:" << testConditions.encoderStart
			 << ",End:" << testConditions.encoderEnd
			 << ",Velocity:" << testConditions.velocity
			 << ",Velocity Base:" << testConditions.velocityBase
			 <<	",Acceleration:"<< testConditions.acceleration;

	qDebug() << "Time(ms), Velocity, Encoder Feedback";

	currentTestAction_ = new AMListAction3(new AMListActionInfo3("Test", "Test"),
										   AMListAction3::Sequential);

	if(!undulatorEncoder_->withinTolerance(testConditions.encoderStart)) {

		// Move undulator step to its starting position
		currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(undulatorEncoder_, testConditions.encoderStart));

		// Wait for undulator step to reach its starting point
		currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(undulatorEncoder_, testConditions.encoderStart, 60, AMControlWaitActionInfo::MatchWithinTolerance));

		currentTestAction_->addSubAction(new AMWaitAction(new AMWaitActionInfo(0.5)));
	}

	// Setup initial conditions
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(undulatorVelocitySetpoint_, testConditions.velocity));
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(undulatorVelocitySetpoint_, testConditions.velocity, 2, AMControlWaitActionInfo::MatchWithinTolerance));
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(undulatorVelocityBase_, testConditions.velocityBase));
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(undulatorVelocityBase_, testConditions.velocityBase, 2, AMControlWaitActionInfo::MatchWithinTolerance));
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(undulatorAcceleration_, testConditions.acceleration));
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(undulatorAcceleration_, testConditions.acceleration, 2, AMControlWaitActionInfo::MatchWithinTolerance));

	// Move to final position.
	currentTestAction_->addSubAction(AMActionSupport::buildControlMoveAction(undulatorEncoder_, testConditions.encoderEnd));

	// Wait for undulator to reach its final destination
	currentTestAction_->addSubAction(AMActionSupport::buildControlWaitAction(undulatorEncoder_, testConditions.encoderEnd, 60, AMControlWaitActionInfo::MatchWithinTolerance));
}
