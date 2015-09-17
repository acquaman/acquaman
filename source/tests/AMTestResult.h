#ifndef AMTESTRESULT_H
#define AMTESTRESULT_H

#include <QString>
class AMTestResult
{
public:
    AMTestResult();
    AMTestResult(bool passed, const QString& failMessage);

    bool passed() const;

    QString message() const;
private:
    bool passed_;
    QString failMessage_;
};

#endif // AMTESTRESULT_H
