#ifndef SGMVELOCITYTESTS_H
#define SGMVELOCITYTESTS_H

#include <QElapsedTimer>
#include <QObject>
class SGMVelocityTests : public QObject
{
	Q_OBJECT
public:
	SGMVelocityTests(QObject* parent = 0);

	void testGratingAngle();
protected slots:
	void onGratingAngleVelocityFeedbackChanged(double value);
protected:
	QElapsedTimer timer_;
};

#endif // SGMVELOCITYTESTS_H
