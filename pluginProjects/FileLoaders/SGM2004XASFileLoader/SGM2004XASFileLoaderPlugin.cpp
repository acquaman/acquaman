#include "SGM2004XASFileLoaderPlugin.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

#include "dataman/AMScan.h"

bool SGM2004XASFileLoaderPlugin::accepts(AMScan *scan){
	if(scan->fileFormat() == "sgm2004")
		return true;
	return false;
}

bool SGM2004XASFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor){
	if(columns2pvNames_.count() == 0) {
		columns2pvNames_.set("eV", "BL1611-ID-1:Energy");
		columns2pvNames_.set("ringCurrent", "PCT1402-01:mA:fbk");
		columns2pvNames_.set("Photodiode", "A1611-4-13:A:fbk");
		columns2pvNames_.set("I0", "A1611-4-14:A:fbk");
		columns2pvNames_.set("TEY", "A1611-4-15:A:fbk");
		columns2pvNames_.set("TFY", "A1611-4-16:A:fbk");
		columns2pvNames_.set("EnergyFeedback", "BL1611-ID-1:Energy:fbk");
		columns2pvNames_.set("pressure", "TCGC1611-423:pressure:fbk");

		columns2pvNames_.set("integrationTime", "A1611I1:cont_interval");
		columns2pvNames_.set("grating", "SG16114I1001:choice");
		columns2pvNames_.set("time", "Absolute-Time-Stamp");

		columns2pvNames_.set("SDDROI0", "MCA1611-01:ROI:0");
		columns2pvNames_.set("SDDROI1", "MCA1611-01:ROI:1");
		columns2pvNames_.set("SDDROI2", "MCA1611-01:ROI:2");
		columns2pvNames_.set("SDDROI3", "MCA1611-01:ROI:3");
		columns2pvNames_.set("SDDDeadtime", "MCA1611-01:DeadFraction");
		columns2pvNames_.set("SDDFileOffset", "MCA1611-01:GetChannels");
	}


	if(defaultUserVisibleColumns_.isEmpty()) {
		defaultUserVisibleColumns_ << "TEY";
		defaultUserVisibleColumns_ << "TFY";
		defaultUserVisibleColumns_ << "I0";
		defaultUserVisibleColumns_ << "Photodiode";
		defaultUserVisibleColumns_ << "EnergyFeedback";
		defaultUserVisibleColumns_ << "ringCurrent";
		defaultUserVisibleColumns_ << "SDD";
	}

	if(!scan)
		return false;

	// Clear the old scan axes to ensure we don't have any extras.
	scan->clearRawDataCompletely();
	scan->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	// information about the scan we hope to locate:
	QString comments;
	QDateTime datetime;
	double integrationTime;
	QString grating;

	// used in parsing the data file
	QString line;
	QStringList lp;

	// names of the columns, taken from headers in the data file. (Will be translated from PV strings into something meaningful)
	QStringList colNames1, colNames2;

	// open the file:
	QFile f(sourceFileInfo.filePath());
	if(!f.open(QIODevice::ReadOnly)) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2004XASFILELOADERPLUGIN_CANNOT_OPEN_FILE, "SGM2004XASFileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	// find out what columns exist. Looking for line starting with '#(1) '
	// find out what information we've got in event ID 1
	line.clear();
	while(!fs.atEnd() && !line.startsWith("#(1) "))
		line = fs.readLine();
	if(fs.atEnd()) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2004XASFILELOADERPLUGIN_BAD_FORMAT_NO_EVENT1_HEADER, "SGM2004FileLoader parse error while loading scan data from file. Missing #(1) event line."));
		return false;	// bad format; missing the #1 event header
	}
	colNames1 = line.split(QChar(' '));
	// the first column is not a column name, it's just the event description header ("#(1)")
	colNames1.removeFirst();
	for(int i=0; i<colNames1.count(); i++)
		pv2columnName(colNames1[i]);

	// find out what information we've got in event ID 2
	fs.seek(0);
	line.clear();
	while(!fs.atEnd() && !line.startsWith("#(2) "))
		line = fs.readLine();
	if(fs.atEnd()) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2004XASFILELOADERPLUGIN_BAD_FORMAT_NO_EVENT2_HEADER, "SGM2004FileLoader parse error while loading scan data from file. Missing #(2) event line."));
		return false;	// bad format; missing the #2 event header
	}
	colNames2 = line.split(QChar(' '));
	// the first column is not a column name, it's just the event description header ("#(1)")
	colNames2.removeFirst();
	for(int i=0; i<colNames2.count(); i++)
		pv2columnName(colNames2[i]);


	// ensure that we have the basic "eV" column
	int eVIndex = colNames1.indexOf("eV");
	if(eVIndex < 0) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2004XASFILELOADERPLUGIN_BAD_FORMAT_NO_ENERGY_COLUMN, "SGM2004FileLoader parse error while loading scan data from file. I couldn't find the energy (eV) column."));
		return false;	// bad format; no primary column

	}


	// clear the existing raw data (and raw data sources, if we're supposed to)
	scan->clearRawDataPointsAndMeasurements();


	// There is a rawData scan axis called "eV" created in the constructor.  AMAxisInfo("eV", 0, "Incident Energy", "eV")
	/// \todo What if there isn't? Should we check, and create the axis if none exist? What if there's more than one scan axis? Can't remove from AMDataStore... [The rest of this code assumes a single scan axis]


	bool postSddFileOffset = false;
	QString spectraFile = "";
	// add scalar (0D) measurements to the raw data store, for each data column.  If setRawDataSources is true, also add raw data sources to the scan, which expose this data.
	foreach(QString colName, colNames1) {
		if(colName != "eV" && colName != "Event-ID") {
			if(colName == "SDDFileOffset"){
				foreach(QString afp, scan->additionalFilePaths())
					if(afp.contains("_spectra.dat"))
						spectraFile = afp;
				if(spectraFile != ""){
					postSddFileOffset = true;
					AMAxisInfo sddEVAxisInfo("energy", 1024, "SDD Energy", "eV");
					QList<AMAxisInfo> sddAxes;
					sddAxes << sddEVAxisInfo;
					AMMeasurementInfo sddInfo("SDD", "Silicon Drift Detector", "counts", sddAxes);
					if(scan->rawData()->addMeasurement(sddInfo)){
						/// \todo Throw an errorMon out?
						//qdebug() << "Added measurement " << scan->rawData()->measurementAt(scan->rawData()->measurementCount()-1).name;
					}
				}
			}
			else if(scan->rawData()->addMeasurement(AMMeasurementInfo(colName, colName))){
				/// \todo Throw an errorMon out?
				//qdebug() << "Added measurement " << scan->rawData()->measurementAt(scan->rawData()->measurementCount()-1).name;
			}
		}
	}
	if(postSddFileOffset){
		scan->rawData()->addMeasurement(AMMeasurementInfo("SDDFileOffset", "SDDFileOffset"));
//		if(scan->rawData()->addMeasurement(AMMeasurementInfo("SDDFileOffset", "SDDFileOffset")))
//			qdebug() << "Added measurement " << scan->rawData()->measurementAt(scan->rawData()->measurementCount()-1).name;
	}
	int sddOffsetIndex = colNames1.indexOf("SDDFileOffset");

	// read all the data. Add to data columns or scan properties depending on the event-ID.

	int eVAxisIndex = 0;	// counter, incremented for every data point along the scan (eV) axis.
	while(!fs.atEnd()) {

		line = fs.readLine();

		// event id 1.  If the line starts with "1," and there are the correct number of columns:
		if(line.startsWith("1,") && (lp = line.split(',')).count() == colNames1.count() ) {

			scan->rawData()->beginInsertRows(1, -1);
			scan->rawData()->setAxisValue(0, eVAxisIndex, lp.at(eVIndex).toDouble()); // insert eV

			// add data from all columns (but ignore the first (Event-ID) and the eV column)
			int measurementId = 0;
			for(int i=1; i<colNames1.count(); i++) {
				if(i == sddOffsetIndex){
					scan->rawData()->setValue(eVAxisIndex, scan->rawData()->idOfMeasurement("SDDFileOffset"), AMnDIndex(), lp.at(i).toDouble());
					measurementId++;
				}
				else if(i!=eVIndex) {
					scan->rawData()->setValue(eVAxisIndex, measurementId++, AMnDIndex(), lp.at(i).toDouble());
				}
			}

			eVAxisIndex++;
			scan->rawData()->endInsertRows();
		}

		// event id 2.  If the line starts with "# 2," and there are the correct number of columns:
		else if(line.startsWith("# 2,") && (lp = line.split(',')).count() == colNames2.count() ) {

			// see if we recognize any of the translated column names as useful:
			for(int i=1; i<colNames2.count(); i++) {

				if(colNames2.at(i) == "time")
					datetime = QDateTime::fromTime_t(uint(lp.at(i).toDouble()));
				if(colNames2.at(i) == "grating")
					grating = lp.at(i);
				if(colNames2.at(i) == "integrationTime")
					integrationTime = lp.at(i).toDouble();
			}
		}
	}


	if(spectraFile != ""){
		QFileInfo spectraFileInfo;
		spectraFileInfo.setFile(spectraFile);
		if(spectraFileInfo.isRelative())
			spectraFileInfo.setFile(userDataFolder + "/" + spectraFile);
		QFile sf(spectraFileInfo.filePath());
		if(!sf.open(QIODevice::ReadOnly)) {
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2004XASFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE, "SGM2004FileLoader parse error while loading scan data from file. Missing SDD spectra file."));
			return false;
		}

		int startByte = 0;
		int endByte = 0;
		int specCounter = 0;
		int scanSize = scan->rawData()->scanSize(0);
		int fileOffsetMeasurementId = scan->rawData()->idOfMeasurement("SDDFileOffset");
		int sddMeasurementId = scan->rawData()->idOfMeasurement("SDD");
		int sddSize = scan->rawData()->measurementAt(sddMeasurementId).size(0);
		int* specValues = new int[sddSize];

		if(scanSize < 2){
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2004XASFILELOADERPLUGIN_SPECTRA_FILE_SIZE_TOO_SMALL, "SGM2004FileLoader parse error while loading scan data from file. SDD cannot have fewer than 2 points."));
			return false;
		}

		for(int x = 0; x < scanSize; x++){
			if(x == scanSize-1){
				endByte += endByte-startByte; //Assumes the last two are the same size
				startByte = scan->rawData()->value(AMnDIndex(x), fileOffsetMeasurementId, AMnDIndex());
			}
			else{
				startByte = scan->rawData()->value(AMnDIndex(x), fileOffsetMeasurementId, AMnDIndex());
				endByte = scan->rawData()->value(AMnDIndex(x+1), fileOffsetMeasurementId, AMnDIndex());
			}

			sf.seek(startByte);
			QByteArray row = sf.read( (qint64)(endByte-startByte) );

			// optimized parsing of the spectrum file. Avoid QTextStream because of unicode conversion. Avoid memory allocation as much as possible. The row is a list of integers, separated by commas and/or spaces.
			int rowLength = row.length();
			bool insideWord = false;
			QString word;
			word.reserve(12);
			for(int rowb = 0; rowb<rowLength; rowb++) {
				char c = row.at(rowb);
				if(c == ' ' || c == ',') {
					// the end of a word
					if(insideWord) {
						//scan->rawData()->setValue(AMnDIndex(x), sddMeasurementId, specCounter++, word.toInt());
						if(specCounter < sddSize)
							specValues[specCounter++] = word.toInt();
						word.clear();
						insideWord = false;
					}
				}
				else {
					word.append(c);
					insideWord = true;
				}
			}
			if(insideWord) // possibly the last word (with no terminators after it)
				if(specCounter < sddSize)
					specValues[specCounter++] = word.toInt();

			// By now we should have specCounter = sddSize. If there wasn't sufficient values in the spectra file to fill the whole measurement array...
			if(specCounter < sddSize)
				memset(specValues+specCounter, 0,  (sddSize-specCounter)*sizeof(int));

			// insert the detector values (all at once, for performance)
			scan->rawData()->setValue(x, sddMeasurementId, specValues);

			// Check specCounter is the right size... Not too big, not too small.
			if(specCounter != sddSize) {
				errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Alert, SGM2004XASFILELOADERPLUGIN_BAD_FORMAT_CORRUPTED_SPECTRA_FILE, QString("SGM2004FileLoader found corrupted data in the SDD spectra file '%1' on row %2. There should be %3 elements in the spectra, but we only found %4").arg(spectraFile).arg(x).arg(sddSize).arg(specCounter)));
			}

			specCounter = 0;
		}
		delete specValues;
	}



	/// Not supposed to create the raw data sources.  Do an integrity check on the pre-existing data sources instead... If there's a raw data source, but it's pointing to a non-existent measurement in the data store, that's a problem. Remove it.  \todo Is there any way to incorporate this at a higher level, so that import-writers don't need to bother?
	for(int i=0; i<scan->rawDataSources()->count(); i++) {
		if(scan->rawDataSources()->at(i)->measurementId() >= scan->rawData()->measurementCount()) {
			errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Debug, SGM2004XASFILELOADERPLUGIN_DATA_COLUMN_MISMATCH, QString("The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
			scan->deleteRawDataSource(i);
		}
	}

	return true;
}

bool SGM2004XASFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "sgm2004");
}

Q_EXPORT_PLUGIN2(SGM2004XASFileLoaderFactory, SGM2004XASFileLoaderFactory)

