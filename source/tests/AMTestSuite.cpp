#include "AMTestSuite.h"

AMTestSuite *AMTestSuite::instance()
{
    if(!instance_) {
        instance_ = new AMTestSuite();
    }

    return instance_;
}

void AMTestSuite::runTests()
{
    testResults_.clear();
    foreach(QString key, classTests_.keys()) {

        AMClassTest* currentTest = classTests_.value(key);
        currentTest->runTests();
    }
}

AMTestSuite::AMTestSuite()
{
    instance_ = 0;
}



