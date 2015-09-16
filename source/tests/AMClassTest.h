#ifndef AMCLASSTEST_H
#define AMCLASSTEST_H

#include <QObject>
#include <QVector>

#include "AMTestResult.h"
#include "AMTestSuite.h"
#define AM_TEST_EQUAL(expected, actual, message) \
    testResults_.append(AMTestResult((actual == expected), message))

#define AM_TEST_LESS_THAN(expected, actual, message) \
    testResults_.append(AMTestResult((actual < expected), message))

#define AM_TEST_LESS_THAN_OR_EQUAL(expected, actual, message) \
    testResults_.append(AMTestResult((actual <= expected), message))

#define AM_TEST_GREATER_THAN(expected, actual, message) \
    testResults_.append(AMTestResult((expected > actual), message))

#define AM_TEST_GREATER_THAN_OR_EQUAL(expected, actual, message) \
    testResults_.append(AMTestResult((expected >= actual), message))

#define AM_TEST_TRUE(value, message) \
    testResults_.append(value, message)

#define AM_TEST_FALSE(value, message) \
    testResults_.append(!value, message)


class AMClassTest
{
    Q_OBJECT
public:
    explicit AMClassTest(QObject *parent = 0);

    QVector<AMTestResult> runTests();

protected:
    virtual void preTestsRun() {}

    virtual void postTestsRun() {}

    virtual void testRunImplementation() = 0;

    QVector<AMTestResult> testResults_;
signals:

public slots:
};

#endif // AMCLASSTEST_H
