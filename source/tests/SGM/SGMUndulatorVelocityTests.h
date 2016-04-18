#ifndef SGMUNDULATORVELOCITYTESTS_H
#define SGMUNDULATORVELOCITYTESTS_H

#include <QTimer>
#include <QElapsedTimer>
#include <QObject>
class AMControl;
class AMListAction3;
class AMControlSet;
class UndulatorVelocityTestConditions
{
public:
	UndulatorVelocityTestConditions(double encStart, double encEnd, double velo, double veloBase, double accel);

	double encoderStart;
	double encoderEnd;
	double velocity;
	double velocityBase;
	double acceleration;
};

class SGMUndulatorVelocityTests : public QObject
{
	Q_OBJECT
public:
	SGMUndulatorVelocityTests(QObject* parent = 0);

	void testUndulator();
protected slots:
	void onTimerTimeout();
	void onAllControlsConnected(bool);
	void runNextTest();
	void epicsDelayElapsed();
	void onTestStarted();
protected:
	void buildTest(const UndulatorVelocityTestConditions& testConditions);

	bool connectedOnce_;
	QTimer timer_;
	QTimer epicsDelay_;
	int interval_;


	AMControl* undulatorEncoder_;

	AMControl* undulatorVelocitySetpoint_;
	AMControl* undulatorVelocityFeedback_;
	AMControl* undulatorVelocityBase_;
	AMControl* undulatorAcceleration_;

	AMControlSet* allControls_;
	AMListAction3* currentTestAction_;

	QList<UndulatorVelocityTestConditions> testQueue_;
};

#endif // SGMUNDULATORVELOCITYTESTS_H
