#ifndef AMTESTSUITE_H
#define AMTESTSUITE_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QDebug>

#include "AMTestResult.h"
class AMClassTest;
class AMTestSuite
{

public:
    static AMTestSuite& instance();

    template <class T>
    void registerClassTest()
    {
       const QMetaObject* classMetaObject = &(T::staticMetaObject);
       qDebug() << "Registering class test with name " << classMetaObject->className();

       if(!classTests_.contains(classMetaObject->className())) {
           QObject* qObjectInstance = classMetaObject->newInstance();
           AMClassTest* classTestInstance = qobject_cast<AMClassTest*>(qObjectInstance);
           if(classTestInstance) {
               classTests_.insert(classMetaObject->className(), classTestInstance);
           }
       }

    }

    void runTests();

    QVector<AMTestResult> testResults() const;

private:

    AMTestSuite() {}
    AMTestSuite(const AMTestSuite&);
    void operator=(const AMTestSuite&);

    QHash<QString, AMClassTest*> classTests_;
    QVector<AMTestResult> testResults_;
};

#include <QDebug>
template <class T>
struct RegisterTest
{
    RegisterTest()
    {
        qDebug() << "Registering class";
        AMTestSuite::instance().registerClassTest<T>();
    }
};

#endif // AMTESTSUITE_H
