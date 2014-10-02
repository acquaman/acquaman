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


#include "VESPERSDbUpgrade1Pt6.h"

#include "util/AMErrorMonitor.h"

VESPERSDbUpgrade1Pt6::VESPERSDbUpgrade1Pt6(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt6::upgradeFromTags() const
{
	return QStringList() << "VESPERSDbUpgrade1.5";
}

bool VESPERSDbUpgrade1Pt6::upgradeNecessary() const
{
	return true;
}

bool VESPERSDbUpgrade1Pt6::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	// 1) Create the AMRegionOfInterest_table.
	QStringList names;
	names << "AMDbObjectType" << "thumbnailCount" << "thumbnailFirstId" << "name" << "energy" << "lowerBound" << "upperBound";
	QStringList types;
	types << "TEXT" << "INTEGER" << "INTEGER" << "TEXT" << "REAL" << "REAL" << "REAL";

	if (!databaseToUpgrade_->tableExists("AMRegionOfInterest_table") && !databaseToUpgrade_->ensureTable("AMRegionOfInterest_table", names, types)){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Was unable to create the AMRegionOfInterest table.");
		return false;
	}

	// 2) Fill the new region table.
	QVariantList ids = databaseToUpgrade_->retrieve("AMROIInfo_table", "id");
	QVariantList regionNames = databaseToUpgrade_->retrieve("AMROIInfo_table", "name");
	QVariantList energies = databaseToUpgrade_->retrieve("AMROIInfo_table", "energy");
	QVariantList lowerBounds = databaseToUpgrade_->retrieve("AMROIInfo_table", "low");
	QVariantList upperBounds = databaseToUpgrade_->retrieve("AMROIInfo_table", "high");

	for (int i = 0, size = ids.size(); i < size; i++){

		int newId = databaseToUpgrade_->insertOrUpdate(0, "AMRegionOfInterest_table", names,
													   QVariantList()
													   << "AMRegionOfInterest"
													   << 0
													   << QVariant()
													   << regionNames.at(i)
													   << energies.at(i)
													   << lowerBounds.at(i)
													   << upperBounds.at(i));

		if (newId == 0){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, 0, "Was unable to copy the data from AMROIInfo columns to the new AMRegionOfInterest table.");
			return false;
		}
	}

	// 3) Create the associated table for regions of interest, VESPERSScanConfigurationDbObject_table.
	names = QStringList() << "id1" << "table1" << "id2" << "table2";
	types = QStringList() << "INTEGER" << "TEXT" << "INTEGER" << "TEXT";

	if (!databaseToUpgrade_->ensureTable("VESPERSScanConfigurationDbObject_table_regionsOfInterest", names, types)){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Was unable to create the VESPERSScanConfigurationDbObject_table_regionsOfInterest table.");
		return false;
	}

	if (databaseToUpgrade_->tableExists("VESPERSScanConfigurationDbObject_table")){

		QString firstElement = databaseToUpgrade_->retrieve(1, "VESPERSScanConfigurationDbObject_table", "roiInfoList").toString();

		// It is possible that there are no entries if it is a REALLY old database.
		if (!firstElement.isEmpty()){

			int firstElementId = firstElement.split(";").last().toInt();
			int correspondingId = 1;

			if (firstElementId != 1)
				correspondingId = databaseToUpgrade_->objectsMatching("AMROIInfoList_table_roiInfos", "id1", firstElementId).first();	// We only want the id of the where we need to start copying.

			QVariantList roiInfosId1Column = databaseToUpgrade_->retrieve("AMROIInfoList_table_roiInfos", "id1");
			QVariantList roiInfosId2Column = databaseToUpgrade_->retrieve("AMROIInfoList_table_roiInfos", "id2");

			for (int i = correspondingId-1, size = roiInfosId1Column.size(); i < size; i++)
				databaseToUpgrade_->insertOrUpdate(0, "VESPERSScanConfigurationDbObject_table_regionsOfInterest",
												   QStringList()
												   << "id1"
												   << "table1"
												   << "id2"
												   << "table2",
												   QVariantList()
												   << roiInfosId1Column.at(i)
												   << "VESPERSScanConfigurationDbObject_table"
												   << roiInfosId2Column.at(i)
												   << "AMRegionOfInterest_table"
												   );
		}

		if (!AMDbUpgradeSupport::removeColumn(databaseToUpgrade_, "VESPERSScanConfigurationDbObject_table", "roiInfoList")){

			databaseToUpgrade_->rollbackTransaction();
			AMErrorMon::alert(this, 0, "Was unable to remove the roiInfoList column from the VESPERSScanConfigurationDbObject_table_regionsOfInterest table.");
			return false;
		}
	}

	// 4)  Adding AMRegionOfInterest to the AMDbObjectType table.
	//////////////////////////////////////////

	QStringList dboColumnNames = QStringList() << "AMDbObjectType" << "tableName" << "description" << "version" << "inheritance";
	QVariantList dboValues = QVariantList() << "AMRegionOfInterest" << "AMRegionOfInterest_table" << "Region Of Interest" << 1 << "AMRegionOfInterest;AMDbObject;QObject";

	if (!databaseToUpgrade_->columnExists("AMDbObjectTypes_table", "inheritance"))
		databaseToUpgrade_->ensureColumn("AMDbObjectTypes_table", "inheritance", "TEXT");

	int newId = databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_table", dboColumnNames, dboValues);

	if (newId == 0){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Could not add the new VESPERSScanConfigurationDbObject entry to the AMDbObjectTypes table.");
		return false;
	}

	// 6)  Filling out the other support tables.
	//////////////////////////////////////////

	// First: allColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "name") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "energy") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "lowerBound") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_allColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "upperBound") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Could not add the entries for VESPERSScanConfigurationDbObject to the allColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	// Second: visibleColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "name") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "energy") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "lowerBound") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_visibleColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "upperBound") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Could not add the entries for VESPERSScanConfigurationDbObject to the visibleColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	// Third: loadColumns
	if (databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "name") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "energy") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "lowerBound") == 0
			|| databaseToUpgrade_->insertOrUpdate(0, "AMDbObjectTypes_loadColumns", QStringList() << "typeId" << "columnName", QVariantList() << newId << "upperBound") == 0
			){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, 0, "Could not add the entries for VESPERSScanConfigurationDbObject to the loadColumns associated table of AMDbObjectTypes table.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();

	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt6::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt6(databaseNameToUpgrade());

	if (databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt6::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.6";
}

QString VESPERSDbUpgrade1Pt6::description() const
{
	return "Upgrades database by migrating away from old AMROIInfo and AMROIInfoLists to the new AMRegionOfInterest.";
}
