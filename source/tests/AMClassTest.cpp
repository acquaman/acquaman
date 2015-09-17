#include "AMClassTest.h"

AMClassTest::AMClassTest() : QObject(0)
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
