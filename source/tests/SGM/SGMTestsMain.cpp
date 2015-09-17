#include "tests/AMTestSuite.h"
#include "tests/AMTestResult.h"

#include <QVector>
#include <QDebug>
int main()
{
    AMTestSuite::instance().runTests();

    QVector<AMTestResult> results = AMTestSuite::instance().testResults();

    foreach(AMTestResult result, results) {

        if(!result.passed())
            qDebug() << result.message();
    }
}
