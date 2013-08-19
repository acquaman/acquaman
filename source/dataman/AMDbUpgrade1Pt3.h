#ifndef AMDBUPGRADE1PT3_H
#define AMDBUPGRADE1PT3_H

#include "dataman/database/AMDbUpgrade.h"

/// This class is used to upgrade databases from AMSample to AMSamplePre2013. This is necessary to upgrade for improvements for AMSample.
/*
class AMDbUpgrade1Pt3 : public AMDbUpgrade
{
    Q_OBJECT
public:
    /// Constructor calls the base constructor for AMDbUpgrade
    AMDbUpgrade1Pt3(QString databaseNameToUpgrade, QObject *parent = 0);

    /// This upgrade depends on AMDbUpgrade1.2
    virtual QStringList upgradeFromTags() const;

    /// Returns true if the old AMSample AMDbObjectType is present in the AMDbObjectTypes_table AND the table contains the old columns for any of the requested databases.
    virtual bool upgradeNecessary() const;

    /// Iterates over the requested databases and uses dbObjectClassBecomes to make sure that the old AMSample is turned into the new one.
    virtual bool upgradeImplementation();

    /// Creates new copy of this upgrade (caller is responsible for memory).
    virtual AMDbUpgrade* createCopy() const;

    /// Upgrade tag for this upgrade is "AMDbUpgrade1.3".
    virtual QString upgradeToTag() const;

    /// Returns the description of the upgrade
    virtual QString description() const;
};
*/
#endif // AMDBUPGRADE1PT3_H
