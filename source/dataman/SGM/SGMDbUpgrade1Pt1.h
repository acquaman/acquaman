#ifndef SGMDBUPGRADE1PT1_H
#define SGMDBUPGRADE1PT1_H

#include "dataman/database/AMDbUpgrade.h"

class SGMDbUpgrade1Pt1 : public AMDbUpgrade
{
Q_OBJECT
public:
	SGMDbUpgrade1Pt1(QList<AMDatabase*> databasesToUpgrade, QObject *parent = 0);

	virtual QStringList upgradeFromTags();
	virtual bool upgradeNecessary();
	virtual bool upgradeImplementation();
	virtual QString upgradeToTag();
	virtual QString description();
};

#endif // SGMDBUPGRADE1PT1_H
