#include "ALSBL8XASFileLoaderPlugin.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "util/AMErrorMonitor.h"

#include "dataman/AMScan.h"

bool ALSBL8XASFileLoaderPlugin::accepts(AMScan *scan){
	return (scan->fileFormat() == "alsBl8Xas");
}

bool ALSBL8XASFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor){

	if(columns2fileFormatHeaders_.count() == 0) {
		columns2fileFormatHeaders_.set("eV", "Mono Energy");
		columns2fileFormatHeaders_.set("ringCurrent", "Beam Current");
		columns2fileFormatHeaders_.set("I0", "Counter 0");
		columns2fileFormatHeaders_.set("TEY", "Counter 1");
		columns2fileFormatHeaders_.set("TFY", "Counter 2");
	}

	if(!scan)
		return false;

	// Clear the old scan axes to ensure we don't have any extras.
	scan->clearRawDataCompletely();
	scan->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative()){
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());
	}

	// used in parsing the data file
	QString line;
	QStringList lp;

	// names of the columns, taken from headers in the data file.
	QStringList colNames1;

	// open the file:
	QFile f(sourceFileInfo.filePath());
	if(!f.open(QIODevice::ReadOnly)) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, ALSBL8XASFileLoaderPLUGIN_CANNOT_OPEN_FILE, "ALSBL8XASFileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	// find out what columns exist. Looking for line starting with 'Time (s)'
	while(!fs.atEnd() && !line.startsWith("Time (s)"))
		line = fs.readLine();
	if(fs.atEnd()) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, ALSBL8XASFileLoaderPLUGIN_BAD_FORMAT_NO_EVENT1_HEADER, "ALSBL8XASFileLoader parse error while loading scan data from file. Missing the Column Header line."));
		return false;	// bad format. Missing the column header
	}
	colNames1 = line.split(QChar('\t'));

	// translate header names into meaningful column names (ex: "Counter 0" ---> "I0".... "Counter1"--->"tey")
	for(int i=0; i<colNames1.count(); i++)
		header2columnName(colNames1[i]);

	// ensure that we have the basic "eV" column
	int eVIndex = colNames1.indexOf("eV");
	if(eVIndex < 0) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, ALSBL8XASFileLoaderPLUGIN_BAD_FORMAT_NO_ENERGY_COLUMN, "ALSBL8XASFileLoader parse error while loading scan data from file. I couldn't find the energy (eV) column."));
		return false;	// bad format. No primary column

	}

	scan->clearRawDataPointsAndMeasurements();

	// There is a rawData scan axis called "eV" created in the constructor.  AMAxisInfo("eV", 0, "Incident Energy", "eV")
	/// \todo What if there isn't? Should we check, and create the axis if none exist? What if there's more than one scan axis? Can't remove from AMDataStore... [The rest of this code assumes a single scan axis]

	// add scalar (0D) measurements to the raw data store, for each data column.  Also add raw data sources to the scan, which expose this data.
	foreach(QString colName, colNames1) {
		if(colName != "eV" && colName != "Event-ID") {
			scan->rawData()->addMeasurement(AMMeasurementInfo(colName, colName));	/// \todo nice descriptions for the common column names. Not just 'tey' or 'tfy'.
		}
	}

	int eVAxisIndex = 0;	// counter for each datapoint along the scan axis.

	// read all the data. Add to data columns or scan properties depending on the event-ID.
	while(!fs.atEnd()) {

		line = fs.readLine();

		// Data line. If there are the correct number of columns:
		if( (lp = line.split('\t', QString::SkipEmptyParts)).count() == colNames1.count() ) {

			// append a new datapoint to the data tree (supply primary eV value here)
			scan->rawData()->beginInsertRows(1, -1);
			scan->rawData()->setAxisValue(0, eVAxisIndex, lp.at(eVIndex).toDouble());	// insert eV

			// add all columns (but ignore the eV column)
			int measurementId = 0;
			for(int i=1; i<colNames1.count(); i++) {
				if(i!=eVIndex)
					scan->rawData()->setValue(eVAxisIndex, measurementId++, AMnDIndex(), lp.at(i).toDouble());
			}
			eVAxisIndex++;
			scan->rawData()->endInsertRows();
		}
	}

	/// Not supposed to create the raw data sources.  Do an integrity check on the pre-existing data sources instead... If there's a raw data source, but it's pointing to a non-existent measurement in the data store, that's a problem. Remove it.  \todo Is there any way to incorporate this at a higher level, so that import-writers don't need to bother?

	for(int i=0; i<scan->rawDataSources()->count(); i++) {
		if(scan->rawDataSources()->at(i)->measurementId() >= scan->rawData()->measurementCount()) {
			errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Debug, ALSBL8XASFileLoaderPLUGIN_DATA_COLUMN_MISMATCH, QString("The data in the file didn't match the raw data columns we were expecting. Removing the raw data column '%1')").arg(scan->rawDataSources()->at(i)->name())));
			scan->deleteRawDataSource(i);
		}
	}

	return true;
}

bool ALSBL8XASFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "alsBl8Xas");
}

Q_EXPORT_PLUGIN2(ALSBL8XASFileLoaderFactory, ALSBL8XASFileLoaderFactory)

