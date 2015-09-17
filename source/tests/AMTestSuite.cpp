#include "AMTestSuite.h"
#include "AMClassTest.h"
AMTestSuite &AMTestSuite::instance()
{
    static AMTestSuite instance;

    return instance;
}

void AMTestSuite::runTests()
{
    testResults_.clear();
    foreach(QString key, classTests_.keys()) {

        AMClassTest* currentTest = classTests_.value(key);
        QVector<AMTestResult> results = currentTest->runTests();
        testResults_ << results;
    }
}

QVector<AMTestResult> AMTestSuite::testResults() const
{
    return testResults_;
}




