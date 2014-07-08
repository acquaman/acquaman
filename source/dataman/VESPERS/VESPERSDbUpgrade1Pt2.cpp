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


#include "VESPERSDbUpgrade1Pt2.h"

#include "util/AMErrorMonitor.h"

 VESPERSDbUpgrade1Pt2::~VESPERSDbUpgrade1Pt2(){}
VESPERSDbUpgrade1Pt2::VESPERSDbUpgrade1Pt2(const QString &databaseNameToUpgrade, QObject *parent)
	: AMDbUpgrade(databaseNameToUpgrade, parent)
{
}

QStringList VESPERSDbUpgrade1Pt2::upgradeFromTags() const
{
	return QStringList();
}

bool VESPERSDbUpgrade1Pt2::upgradeNecessary() const
{
	return true;
}

bool VESPERSDbUpgrade1Pt2::upgradeImplementation()
{
	databaseToUpgrade_->startTransaction();

	// Since all three of the tables MUST exist, we'll just go ahead and do the upgrade without any further existence checks.
	if (databaseToUpgrade_->tableExists("VESPERS2DScanConfiguration_table")
			&& databaseToUpgrade_->columnExists("VESPERS2DScanConfiguration_table", "fluorescenceDetectorChoice")
			&& !AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERS2DScanConfiguration_table", "fluorescenceDetectorChoice", "fluorescenceDetector")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_2DCONFIGURATION, "Could not upgrade the 2D scan configuration table.");
		return false;
	}

	if (databaseToUpgrade_->tableExists("VESPERSSpatialLineScanConfiguration_table")
			&& databaseToUpgrade_->columnExists("VESPERSSpatialLineScanConfiguration_table", "fluorescenceDetectorChoice")
			&& !AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSSpatialLineScanConfiguration_table", "fluorescenceDetectorChoice", "fluorescenceDetector")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_LINECONFIGURATION, "Could not upgrade the spatial line scan configuration table.");
		return false;
	}

	if (databaseToUpgrade_->tableExists("VESPERSEXAFSScanConfiguration_table")
			&& databaseToUpgrade_->columnExists("VESPERSEXAFSScanConfiguration_table", "fluorescenceDetectorChoice")
			&& !AMDbUpgradeSupport::changeColumnName(databaseToUpgrade_, "VESPERSEXAFSScanConfiguration_table", "fluorescenceDetectorChoice", "fluorescenceDetector")){

		databaseToUpgrade_->rollbackTransaction();
		AMErrorMon::alert(this, VESPERSDBUPGRADE1PT2_COULD_NOT_UPDATE_EXAFSCONFIGURATION, "Could not upgrade the EXAFS scan configuration table.");
		return false;
	}

	databaseToUpgrade_->commitTransaction();
	return true;
}

AMDbUpgrade *VESPERSDbUpgrade1Pt2::createCopy() const
{
	AMDbUpgrade *copy = new VESPERSDbUpgrade1Pt2(this->databaseNameToUpgrade());

	if (this->databaseToUpgrade())
		copy->loadDatabaseFromName();

	return copy;
}

QString VESPERSDbUpgrade1Pt2::upgradeToTag() const
{
	return "VESPERSDbUpgrade1.2";
}

QString VESPERSDbUpgrade1Pt2::description() const
{
	return "Changes the name of fluorescenceDetectorChoice to fluorescenceDetector.";
}
