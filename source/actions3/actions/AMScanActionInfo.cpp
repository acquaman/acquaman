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


#include "AMScanActionInfo.h"

#include "acquaman/AMScanConfiguration.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"

#include <QStringBuilder>

AMScanActionInfo::AMScanActionInfo(QObject *parent)
	: AMActionInfo3("ScanName", "Description", ":/spectrum.png" , parent)
{
	config_ = 0;
	scanID_ = -1;
}

AMScanActionInfo::AMScanActionInfo(AMScanConfiguration *config, const QString &iconFileName, QObject *parent)
	: AMActionInfo3(config->userScanName(), config->description(), iconFileName, parent)
{
	config_ = config;
	scanID_ = -1;

	connect(config_, SIGNAL(expectedDurationChanged(double)), this, SLOT(setExpectedDuration(double)));
	connect(config_, SIGNAL(configurationChanged()), this, SLOT(onConfigChanged()));
	setExpectedDuration(config_->expectedDuration());

	if(!config_->detailedDescription().isEmpty()){
		setShortDescription(config_->userScanName()%"\n"%config_->description());
		setLongDescription(config_->detailedDescription());
	}
}

AMScanActionInfo::AMScanActionInfo(const AMScanActionInfo &other)
	: AMActionInfo3(other)
{
	config_ = other.config_ ? other.config_->createCopy() : 0;
	scanID_ = -1;

	connect(config_, SIGNAL(expectedDurationChanged(double)), this, SLOT(setExpectedDuration(double)));
	connect(config_, SIGNAL(configurationChanged()), this, SLOT(onConfigChanged()));
	setExpectedDuration(config_->expectedDuration());

	if(!config_->detailedDescription().isEmpty()){
		QString scanName = config_->userScanName();
		if(scanName.isEmpty())
			scanName = other.shortDescription();
		setShortDescription(scanName);
		setLongDescription(config_->detailedDescription());
	}
}

AMScanActionInfo::~AMScanActionInfo()
{
}

AMActionInfo3 *AMScanActionInfo::createCopy() const
{
	AMActionInfo3 *info = new AMScanActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}

const AMScanConfiguration *AMScanActionInfo::configuration() const
{
	if (config_)
		return config_;

	return 0;
}

AMScanConfiguration *AMScanActionInfo::configuration()
{
	if (config_)
		return config_;

	config_ = getConfigurationFromDb();
	connect(config_, SIGNAL(expectedDurationChanged(double)), this, SLOT(setExpectedDuration(double)));

	setExpectedDuration(config_->expectedDuration());
	return config_;
}

QString AMScanActionInfo::dbLoadWarnings() const{
	if(configuration()) {
		return configuration()->dbLoadWarnings();
	} else {
		return QString("An unknown error occured loading the scan configuration");
	}
}

AMDbObject* AMScanActionInfo::dbGetConfig() const{
	return config_;
}

void AMScanActionInfo::dbLoadConfig(AMDbObject *newConfig){
	AMScanConfiguration *configuration = qobject_cast<AMScanConfiguration*>(newConfig);
	if(configuration)
		config_ = configuration;
}

void AMScanActionInfo::onConfigChanged()
{
	setShortDescription(config_->userScanName()%"\n"%config_->description());
	setLongDescription(config_->detailedDescription());
}

AMScanConfiguration *AMScanActionInfo::getConfigurationFromDb() const
{
	// no need to bother if we don't have a scan id to use
	if(scanID_ == -1)
		return 0; //NULL

	// turn off automatic raw-data loading for scans... This will make loading the scan to access it's config much faster.
	bool scanAutoLoadingOn = AMScan::autoLoadData();
	AMScan::setAutoLoadData(false);
	// Dynamically create and load a detailed subclass of AMDbObject from the database... whatever type it is.
	AMDbObject* dbo = AMDbObjectSupport::s()->createAndLoadObjectAt(AMDatabase::database("user"), AMDbObjectSupport::s()->tableNameForClass<AMScan>(), scanID_);
	if(!dbo){

		AMErrorMon::alert(this, AMSCANACTIONINFO_CANNOT_LOAD_FROM_DB, "Could not load scan from the database.");
		return 0; //NULL
	}

	// restore AMScan's auto-loading of data to whatever it was before.
	AMScan::setAutoLoadData(scanAutoLoadingOn);
	// Is it a scan?
	AMScan* scan = qobject_cast<AMScan*>( dbo );
	if(!scan) {

		AMErrorMon::alert(this, AMSCANACTIONINFO_DB_OBJECT_NOT_A_SCAN, "Object loaded from the database was not a scan.");
		dbo->deleteLater();
		return 0; //NULL
	}
	// Does the scan have a configuration?
	AMScanConfiguration* config = scan->scanConfiguration();
	if(!config) {

		AMErrorMon::alert(this, AMSCANACTIONINFO_SCAN_HAS_NO_CONFIGURATION, "Scan does not have a valid scan configuration.");
		scan->deleteLater();
		return 0; //NULL
	}
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	config = config->createCopy();
	scan->deleteLater();

	if(!config){

		AMErrorMon::alert(this, AMSCANACTIONINFO_CREATE_CONFIGURATION_COPY_FAILED, "Failed to create a copy of the scan configuration.");
	}

	return config;
}
