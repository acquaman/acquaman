#ifndef AMDBUPGRADE1PT4_H
#define AMDBUPGRADE1PT4_H

#include "dataman/database/AMDbUpgrade.h"

class AMDbUpgrade1Pt4 : public AMDbUpgrade
{
Q_OBJECT
public:
	/// Constructor calls the base constructor for AMDbUpgrade
	AMDbUpgrade1Pt4(QString databaseNameToUpgrade, QObject *parent = 0);

	/// This upgrade depends on AMDbUpgrade1.2
	virtual QStringList upgradeFromTags() const;

	/// Returns true if AMScan or AMSamplePosition still has the sampleId column and is missing the sample column
	virtual bool upgradeNecessary() const;

	/// Changes the column name from sampleId to sample for AMScan and AMSamplePosition. Changes all instances of "-1" to "" (empty string). Changes all values from "#" to "AMSample_table;#"
	/*!
	  This upgrade also changes the name for AMSample, AMSamplePosition, and AMSamplePlate to AMSamplePre2013, AMSamplePositionPre2013, and AMSamplePlatePre2013
	  */
	virtual bool upgradeImplementation();

	/// Creates new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade* createCopy() const;

	/// Upgrade tag for this upgrade is "AMDbUpgrade1.3".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade
	virtual QString description() const;
};

#endif // AMDBUPGRADE1PT4_H
