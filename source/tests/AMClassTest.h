#ifndef AMCLASSTEST_H
#define AMCLASSTEST_H

#include <QObject>
#include <QVector>

#include "AMTestResult.h"
#include "AMTestSuite.h"
class AMClassTest : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit AMClassTest();

    virtual ~AMClassTest() {}

    QVector<AMTestResult> runTests();

protected:
    virtual void preTestsRun() {}

    virtual void postTestsRun() {}

    virtual void testRunImplementation() {}

    QVector<AMTestResult> testResults_;
signals:

public slots:
};

#define AM_CLASS_TEST(className) \
    static RegisterTest<className> regInSuite##className; \

#include <QDebug>
class example : public AMClassTest {

    Q_OBJECT

public:
    AM_CLASS_TEST(example)
    Q_INVOKABLE example() : AMClassTest() {}

    void testRunImplementation()
    {
        qDebug() << "example test file";
    }
};

#endif // AMCLASSTEST_H
