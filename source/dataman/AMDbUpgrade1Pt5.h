#ifndef AMDBUPGRADE1PT5_H
#define AMDBUPGRADE1PT5_H

#include "dataman/database/AMDbUpgrade.h"

/// This upgrade removes the regions string dependency from AMScanConfigurations and converts them to be able to AMStepScanConfigurations.
class AMDbUpgrade1Pt5 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.
	AMDbUpgrade1Pt5(const QString &databaseNameToUpgrade, QObject *parent = 0);

	/// This upgrade depends on AMDbUpgrade1.4
	virtual QStringList upgradeFromTags() const;

	/// This upgrade is always necessary.
	virtual bool upgradeNecessary() const;

	/// Changes columns inside of all all configurations that depend on AMRegionScanConfiguration.
	/*!
	  This upgrade will find all the scan configurations that use AMRegionScanConfiguration or AM2DScanConfiguration
	  and modifies them to be loaded as AMStepScanConfigurations.
	  */
	virtual bool upgradeImplementation();

	/// Creates new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade* createCopy() const;

	/// Upgrade tag for this upgrade is "AMDbUpgrade1.5".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade
	virtual QString description() const;
};

#endif // AMDBUPGRADE1PT5_H
