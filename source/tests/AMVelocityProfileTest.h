#ifndef AMVELOCITYPROFILETEST_H
#define AMVELOCITYPROFILETEST_H

#include <QStringList>
class AMVelocityProfileTest
{
public:

    AMVelocityProfileTest();

    void performTests();

    QStringList testResults() const;

protected:

    void testIndividualProfile(double baseVelocity,
                               double targetVelocity,
                               double acceleration,
                               double deceleration,
                               double timePeriod,
                               const QList<double>& expectedVelocities,
                               const QList<double>& expectedAccelerations);

    QStringList testResults_;
};

#endif // AMVELOCITYPROFILETEST_H
