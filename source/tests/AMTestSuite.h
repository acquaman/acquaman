#ifndef AMTESTSUITE_H
#define AMTESTSUITE_H

#include <QObject>
#include <QVector>
#include <QHash>
#include "AMClassTest.h"
#include "AMTestResult.h"
class AMTestSuite
{

public:
    AMTestSuite* instance();

    template <class T>
    void registerClassTest(QMetaObject* classMetaObject)
    {
       QMetaObject* classMetaObject = &(T::staticMetaObject);

       if(!classTests_.contains(classMetaObject->className())) {
           classTests_.insert(classMetaObject->className(), classMetaObject->newInstance());
       }

    }

    void runTests();
protected:
    AMTestSuite();

private:
    static AMTestSuite* instance_;

    QHash<QString, AMClassTest*> classTests_;
    QVector<AMTestResult> testResults_;
};

#endif // AMTESTSUITE_H
