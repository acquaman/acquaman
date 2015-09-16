#include "AMClassTest.h"

AMClassTest::AMClassTest(QObject *parent) : QObject(parent)
{

}

QVector<AMTestResult> AMClassTest::runTests()
{
    testResults_.clear();
    preTestsRun();
    testRunImplementation();
    postTestsRun();
    return testResults_;
}
