#include "AMVelocityProfileTest.h"

#include "util/AMVelocityProfile.h"

#include <QDebug>
AMVelocityProfileTest::AMVelocityProfileTest()
{


}

void AMVelocityProfileTest::performTests()
{
    testResults_.clear();

    // Test a symmetrical trapezoid which has time to reach its top speed
    testIndividualProfile(0,
                          10,
                          3,
                          -3,
                          10,
                          QList<double>() << 0 << 3 << 6 << 9 << 10 << 10 << 10 << 9  << 6  << 3 << 0,
                          QList<double>() << 3 << 3 << 3 << 3 << 0  <<  0 <<  0 << -3 << -3 << -3 << -3);


    // Test a symmetrical trapezoid which has time to reach its top speed and has a non zero base velocity
    testIndividualProfile(5,
                          15,
                          3,
                          -3,
                          10,
                          QList<double>() << 5 << 8 << 11 << 14 << 15 << 15 << 15 << 14  << 11  << 8 << 5,
                          QList<double>() << 3 << 3 << 3 << 3 << 0  <<  0 <<  0 << -3 << -3 << -3 << -3);

    // Test an inverted trapezoid, whose base velocity is > its target
    testIndividualProfile(10,
                          0,
                          -3,
                          3,
                          10,
                          QList<double>() << 10 << 7 << 4 << 3 << 0 << 0 << 0 << 3  << 4  << 7 << 10,
                          QList<double>() << -3 << -3 << -3 << -3 << 0  <<  0 <<  0 << 3 << 3 << 3 << 3);


}

QStringList AMVelocityProfileTest::testResults() const
{
    return testResults_;
}

void AMVelocityProfileTest::testIndividualProfile(double baseVelocity,
                                                  double targetVelocity,
                                                  double acceleration,
                                                  double deceleration,
                                                  double timePeriod,
                                                  const QList<double> &expectedVelocities,
                                                  const QList<double> &expectedAccelerations)
{
    AMTrapezoidalVelocityProfile testVelocityProfile(timePeriod,
                                                     baseVelocity,
                                                     targetVelocity,
                                                     acceleration,
                                                     deceleration);

    QVector<double> velocities = testVelocityProfile.velocityMap(1);
    QVector<double> accelerations = testVelocityProfile.accelerationMap(1);
    QString detailString = QString("Base V: %1 Target V: %2 Acceleration: %3 Deceleration: %4 Time Period: %5")
            .arg(baseVelocity)
            .arg(targetVelocity)
            .arg(acceleration)
            .arg(deceleration)
            .arg(timePeriod);

    QString values = "(";
    for(int i = 0; i < velocities.count(); ++i) {
        values.append(QString(" a: %1 e: %2, ").arg(velocities.at(i)).arg(expectedVelocities.at(i)));
    }
    values.append(")");
    qDebug() << values;

//    for(int iVelocity = 0, size = velocities.count();
//        iVelocity < size;
//        ++iVelocity) {

//        if (qAbs(velocities.at(iVelocity) - expectedVelocities.at(iVelocity)) > 0.001) {

//            testResults_.append(QString("Velocity Test Failed for %1\n Expected: %2 Actual: %3")
//                                 .arg(detailString)
//                                 .arg(expectedVelocities.at(iVelocity))
//                                 .arg(velocities.at(iVelocity)));

//        }
//    }

//    for(int iAcceleration = 0, size = accelerations.count();
//        iAcceleration < size;
//        ++iAcceleration) {

//        if (qAbs(accelerations.at(iAcceleration) - expectedAccelerations.at(iAcceleration)) > 0.001) {

//            testResults_.append(QString("Acceleration Test Failed for %1\n Expected: %2 Actual: %3")
//                                 .arg(detailString)
//                                 .arg(expectedAccelerations.at(iAcceleration))
//                                 .arg(accelerations.at(iAcceleration)));

//        }
//    }
}
