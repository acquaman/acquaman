#include "AMTestResult.h"

AMTestResult::AMTestResult(bool passed, const QString &failMessage)
{
    passed_ = passed;
    failMessage_ = failMessage;
}

bool AMTestResult::passed() const
{
    return passed_;
}

QString AMTestResult::message() const
{
    if(passed_) {
        return "Test Passed";
    } else {
        return failMessage_;
    }
}

