/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef VESPERSDBUPGRADE1PT5_H
#define VESPERSDBUPGRADE1PT5_H

#include "dataman/database/AMDbUpgrade.h"

#define VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_VESPERSSCANCONFIGURATIONDBOBJECT_TABLE 389400
#define VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERS2DSCANCONFIGURATION_TABLE 389401
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_2DCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS 389402
#define VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERS2DSCANCONFIGURATION_TABLE 389403
#define VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERSSPATIALLINESCANCONFIGURATION_TABLE 389404
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_SPATIALLINECONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS 389405
#define VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERSSPATIALLINESCANCONFIGURATION_TABLE 389406
#define VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERSEXAFSSCANCONFIGURATION_TABLE 389407
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_EXAFSCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS 389408
#define VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERSEXAFSSCANCONFIGURATION_TABLE 389409
#define VESPERSDBUPGRADE1PT5_COULD_NOT_CREATE_COLUMN_VESPERSENERGYSCANCONFIGURATION_TABLE 389410
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_ENERGYCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS_OR_LOADCOLUMNS 389411
#define VESPERSDBUPGRADE1PT5_COULD_NOT_COPY_DATA_TO_NEW_TABLE_FROM_VESPERSENERGYSCANCONFIGURATION_TABLE 389412
#define VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERS2DSCANCONFIGURATION_TABLE 389413
#define VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERSSPATIALLINESCANCONFIGURATION_TABLE 389414
#define VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERSEXAFSSCANCONFIGURATION_TABLE 389415
#define VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_OLD_COLUMNS_VESPERSENERGYSCANCONFIGURATION_TABLE 389416
#define VESPERSDBUPGRADE1PT5_COULD_NOT_REMOVE_ALL_INSTANCES_ROIS_COLUMN 389417
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRY_VESPERSSCANCONFIGURATIONDBOBJECT_TO_AMDBOBJECTTYPES_TABLE 389418
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_ALLCOLUMNS 389419
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_VISIBLECOLUMNS 389420
#define VESPERSDBUPGRADE1PT5_COULD_NOT_ADD_NEW_ENTRIES_TO_AMDBOBJECTTYPES_TABLE_LOADCOLUMNS 389421

/// This class upgrades the database to use the new VESPERSScanConfigurationDbObject.
/*!
	This databse upgrade does a very major change to how the scan configurations are stored.
	All but VESPERSXRFScanConfiguration now use the VESPERSScanConfigurationDbObject which
	encapsulates many of the standard aspects of a scan configuration, such as the fluorescence
	detectors and which motors are being scanned.  The upgrade works as follows:

		1) Creates a new table for the new VESPERSScanConfigurationDbObjects.
		2) Copies all the pertinent data from the old columns to the new ones and populate all the extra columns with default values.
		3) Removes the now duplicated columns.
		4) Removes the rois column whereever it resides since it is not used anymore.
		5) Make a new entry for the new database object in AMDbObjectTypes and fill it.
		6) Ensures that all associated tables are updated.
  */
class VESPERSDbUpgrade1Pt5 : public AMDbUpgrade
{
	Q_OBJECT

public:
	/// Constructor. Need to provide the name of the database to upgrade.
	VESPERSDbUpgrade1Pt5(const QString &databaseNameToUpgrade, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSDbUpgrade1Pt5();

	/// Indicates the dependencies of this upgrade.  This requires all of the previous upgrades to have been performed already (1.1-1.4).
	virtual QStringList upgradeFromTags() const;

	/// Returns true.  ALL modifications to the structure of the database need to be performed.
	virtual bool upgradeNecessary() const;

	/// Makes all the changes outlined in the class description.
	virtual bool upgradeImplementation();

	/// Creates a new copy of this upgrade (caller is responsible for memory).
	virtual AMDbUpgrade *createCopy() const;

	/// Upgrade tag for this upgrade is "VESPERSDbUpgrade1.5".
	virtual QString upgradeToTag() const;

	/// Returns the description of the upgrade.
	virtual QString description() const;
};

#endif // VESPERSDBUPGRADE1PT5_H
