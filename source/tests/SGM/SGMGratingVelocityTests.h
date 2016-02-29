#ifndef SGMGRATINGVELOCITYTESTS_H
#define SGMGRATINGVELOCITYTESTS_H

#include <QTimer>
#include <QElapsedTimer>
#include <QObject>
class AMControl;
class AMListAction3;
class AMControlSet;
class GratingVelocityTestConditions
{
public:
	GratingVelocityTestConditions(double encStart, double encEnd, double gratingTransTarget, double velo, double veloBase, double accel);

	double encoderStart;
	double encoderEnd;
	double gratingTranslationTarget;
	double velocity;
	double velocityBase;
	double acceleration;
};

class SGMGratingVelocityTests : public QObject
{
	Q_OBJECT
public:
	SGMGratingVelocityTests(QObject* parent = 0);

	void testGratingAngle();
protected slots:
	void onTimerTimeout();
	void onAllControlsConnected(bool);
	void runNextTest();
	void epicsDelayElapsed();
	void onTestStarted();
protected:
	void buildTest(const GratingVelocityTestConditions& testConditions);

	bool connectedOnce_;
	QTimer timer_;
	QTimer epicsDelay_;
	int interval_;


	AMControl* gratingAngleEncoder_;
	AMControl* gratingTranslation_;

	AMControl* gratingAngleVelocitySetpoint_;
	AMControl* gratingAngleVelocityFeedback_;
	AMControl* gratingAngleVelocityBase_;
	AMControl* gratingAngleAcceleration_;

	AMControlSet* allControls_;
	AMListAction3* currentTestAction_;

	QList<GratingVelocityTestConditions> testQueue_;
};

#endif // SGMGRATINGVELOCITYTESTS_H
