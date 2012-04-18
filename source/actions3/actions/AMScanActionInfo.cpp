#include "AMScanActionInfo.h"

#include "acquaman/AMScanConfiguration.h"
#include "dataman/AMScan.h"
#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"

AMScanActionInfo::AMScanActionInfo(AMScanConfiguration *config, const QString &iconFileName, QObject *parent)
	: AMActionInfo3(config->name(), config->description(), iconFileName, parent)
{
	config_ = config;
	scanID_ = -1;
}

AMScanActionInfo::AMScanActionInfo(const AMScanActionInfo &other)
	: AMActionInfo3(other)
{
	config_ = other.config_ ? other.config_->createCopy() : 0;
	scanID_ = other.scanID_;
}

AMScanActionInfo::~AMScanActionInfo()
{
	if (config_)
		delete config_;
}

const AMScanConfiguration *AMScanActionInfo::config() const
{
	if (config_)
		return config_;

	return 0;
}

AMScanConfiguration *AMScanActionInfo::config()
{
	if (config_)
		return config_;

	config_ = getConfigurationFromDb();
	return config_;
}

AMScanConfiguration *AMScanActionInfo::getConfigurationFromDb() const
{
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
		delete dbo;
		return 0; //NULL
	}
	// Does the scan have a configuration?
	AMScanConfiguration* config = scan->scanConfiguration();
	if(!config) {

		AMErrorMon::alert(this, AMSCANACTIONINFO_SCAN_HAS_NO_CONFIGURATION, "Scan does not have a valid scan configuration.");
		scan->release();
		return 0; //NULL
	}
	// need to create a copy of the config so we can delete the scan (and hence the config instance owned by the scan). The view will take ownership of the copy.
	config = config->createCopy();
	scan->release();

	if(!config){

		AMErrorMon::alert(this, AMSCANACTIONINFO_CREATE_CONFIGURATION_COPY_FAILED, "Failed to create a copy of the scan configuration.");
	}

	return config;
}
