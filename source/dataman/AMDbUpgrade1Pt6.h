#ifndef AMDBUPGRADE1PT6_H
#define AMDBUPGRADE1PT6_H

#include "dataman/database/AMDbUpgrade.h"

#define AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_BIOXAS_XRF_CONFIGURATION 980000
#define AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_VESPERS_XRF_CONFIGURATION 980001
#define AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_IDEAS_XRF_CONFIGURATION 980002
#define AMDBUPGRADE1PT6_FAILED_TO_MIGRATE_SXRMB_XRF_CONFIGURATION 980003
#define AMDBUPGRADE1PT6_COULD_NOT_CREATE_AMXRFSCANCONFIGURATION_TABLE 980004

/// This upgrade moves all the beamline specific XRF scan configurations to the new AMXRFScanConfiguration.
class AMDbUpgrade1Pt6 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor.
	AMDbUpgrade1Pt6(const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	virtual ~AMDbUpgrade1Pt6();

	/// This upgrade does not depend on previous upgrades.
	virtual QStringList upgradeFromTags() const;

	/// This upgrade is necessary if the IDEAS, SXRMB, BioXAS, or VESPERS XRF scan configuration tables exist.
	virtual bool upgradeNecessary() const;

	/// Migrates all the beamline XRF scan configurations and moves them to the new AM version.
	/*!
	  This upgrade will essentially move the contents of the old XRF scan configuration table
	  to the new one and update the scan table to reference the appropriate scan configuration.
	  */
	virtual bool upgradeImplementation();

	/// Creates new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "AMDbUpgrade1.6".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade
	virtual QString description() const;

private:
	/// Updates a single beamlines table.  This is sufficient because beamline databases haven't been moving across a user database won't have more than one.
	bool updateSingleXRFTable(const QString &tableName);
};

#endif // AMDBUPGRADE1PT6_H
