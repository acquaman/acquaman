#include "AMSettings.h"
#include <QtTest/QtTest>
#include "beamline/SGMBeamline.h"
#include "dataman/AMDatabase.h"
#include "dataman/AMXASScan.h"

class AcquamanTest: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase()
	{
		AMSettings::load();
		AMUserSettings::load();
		AMPVNames::load();
		AMScan::dbPrepareTables(AMDatabase::userdb());
		SGMBeamline::sgm();
		QTest::qWait(2000);
		QVERIFY(SGMBeamline::sgm()->isConnected());
	}

	void scanConfigurations()
	{

		QVERIFY(1 == 1);
	}

	void mySecondTest()
	{
		QVERIFY(1 != 2);
	}

	void cleanupTestCase()
	{
		AMDatabase::releaseUserDb();
	}
};

QTEST_MAIN(AcquamanTest)
#include "tests.moc"
