#ifndef AMDBUPGRADE1PT5_H
#define AMDBUPGRADE1PT5_H

#include "dataman/database/AMDbUpgrade.h"

#define AMDBUPGRADE1PT5_REGION_CONFIGURATION_QUERY_FAILED 979000
#define AMDBUPGRADE1PT5_COULD_NOT_CREATE_TABLE 979001
#define AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS 979002
#define AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_CONFIGURATION_ASSOCIATED_TABLE 979003
#define AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS_REGION 979004
#define AMDBUPGRADE1PT5_COULD_NOT_ADD_SCAN_AXIS_EXAFS_REGION 979005
#define AMDBUPGRADE1PT5_COULD_NOT_ADD_TO_SCAN_AXIS_ASSOCIATED_TABLE 979006
#define AMDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS 979007
#define AMDBUPGRADE1PT5_COULD_NOT_UPDATE_INHERITANCE 979008
#define AMDBUPGRADE1PT5_2D_CONFIGURATION_QUERY_FAILED 979009

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

private:
	/// Returns the name of the axis based on the table name and id of the configuration of interest.
	QString axisName(const QString &tableName, int id) const;
};

#endif // AMDBUPGRADE1PT5_H
