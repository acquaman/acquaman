#include <QtTest/QtTest>

class TestBeamline: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase()
	{ qDebug("called before everything else"); }
	void cleanupTestCase()
	{ qDebug("called after myFirstTest and mySecondTest"); }
	void myFirstTest()
	{ QVERIFY(1 == 1); }
	void mySecondTest()
	{ QVERIFY(1 != 2); }

};
