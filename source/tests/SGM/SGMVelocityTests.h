#ifndef SGMVELOCITYTESTS_H
#define SGMVELOCITYTESTS_H

#include <QTimer>
#include <QElapsedTimer>
#include <QObject>
class AMSinglePVControl;
class AMListAction3;
class AMControlSet;
class VelocityTestConditions
{
public:
	VelocityTestConditions(double encStart, double encEnd, double gratingTranslationTarget, double velo, double veloBase, double accel);

	double encoderStart;
	double encoderEnd;
	double gratingTranslationTarget;
	double velocity;
	double velocityBase;
	double acceleration;
};

class SGMVelocityTests : public QObject
{
	Q_OBJECT
public:
	SGMVelocityTests(QObject* parent = 0);

	void testGratingAngle();
protected slots:
	void onTimerTimeout();
	void onAllControlsConnected(bool);
	void runNextTest();
	void epicsDelayElapsed();
protected:
	void buildTest(const VelocityTestConditions& testConditions);

	bool connectedOnce_;
	QTimer timer_;
	QTimer epicsDelay_;
	int interval_;

	AMSinglePVControl* gratingAngleVelocityFeedback_;
	AMSinglePVControl* gratingAngleEncoderFeedback_;
	AMSinglePVControl* gratingAngleVelocity_;
	AMSinglePVControl* gratingAngleVelocityBase_;
	AMSinglePVControl* gratingAngleAcceleration_;
	AMSinglePVControl* gratingAngleEncoderSetpoint_;
	AMSinglePVControl* gratingTranslationSetpoint_;
	AMSinglePVControl* gratingTranslationFeedback_;

	AMControlSet* allControls_;
	AMListAction3* currentTestAction_;

	QList<VelocityTestConditions> testQueue_;
};

#endif // SGMVELOCITYTESTS_H
