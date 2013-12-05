#include "SGM2010FastFileLoaderPlugin.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

#include "dataman/AMScan.h"
#include "acquaman/AMScanConfiguration.h"

bool SGM2010FastFileLoaderPlugin::accepts(AMScan *scan){
	if(scan->fileFormat() == "sgm2010Fast")
		return true;
	return false;
}

bool SGM2010FastFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor){
	if(columns2pvNames_.count() == 0) {
		columns2pvNames_.set("eV", "BL1611-ID-1:Energy");
		columns2pvNames_.set("scaler_fileOffset", "BL1611-ID-1:mcs:scan");
		columns2pvNames_.set("time", "Absolute-Time-Stamp");
		columns2pvNames_.set("grating", "SG16114I1001:choice");
		columns2pvNames_.set("encoder", "SMTR16114I1002:enc:fbk");
	}

	if(defaultUserVisibleColumns_.isEmpty()) {
		defaultUserVisibleColumns_ << "TEY";
		defaultUserVisibleColumns_ << "TFY";
		defaultUserVisibleColumns_ << "I0";
		defaultUserVisibleColumns_ << "Photodiode";
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
	QString grating;
	bool hasEncoderInfo = false;
	int encoderEndpoint = 0;
	int encoderStartPoint = 0;

	// used in parsing the data file
	QString line;
	QStringList lp;

	// names of the columns, taken from headers in the data file. (Will be translated from PV strings into something meaningful)
	QStringList colNames1, colNames2;

	// open the file:
	QFile f(sourceFileInfo.filePath());
	if(!f.open(QIODevice::ReadOnly)) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2010FASTFILELOADERPLUGIN_CANNOT_OPEN_FILE, "SGM2010FastFileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	// find out what columns exist. Looking for line starting with '#(1) '
	// find out what information we've got in event ID 1
	line.clear();
	while(!fs.atEnd() && !line.startsWith("#(1) "))
		line = fs.readLine();
	if(fs.atEnd()) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2010FASTFILELOADERPLUGIN_BAD_FORMAT_NO_EVENT1_HEADER, "SGM2010FastFileLoader parse error while loading scan data from file. Missing #(1) event line."));
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
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2010FASTFILELOADERPLUGIN_BAD_FORMAT_NO_EVENT2_HEADER, "SGM2010FastFileLoader parse error while loading scan data from file. Missing #(2) event line."));
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
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2010FASTFILELOADERPLUGIN_BAD_FORMAT_NO_ENERGY_COLUMN, "SGM2010FastFileLoader parse error while loading scan data from file. I couldn't find the energy (eV) column."));
		return false;	// bad format; no primary column

	}

	// clear the existing raw data (and raw data sources, if we're supposed to)
	scan->clearRawDataPointsAndMeasurements();

	scan->rawData()->addMeasurement(AMMeasurementInfo("TEY", "TEY"));
	scan->rawData()->addMeasurement(AMMeasurementInfo("TFY", "TFY"));
	scan->rawData()->addMeasurement(AMMeasurementInfo("I0", "I0"));
	scan->rawData()->addMeasurement(AMMeasurementInfo("Photodiode", "Photodiode"));

	while(!fs.atEnd()) {
		line = fs.readLine();
		if(line.startsWith("1,") && (lp = line.split(',')).count() == colNames1.count() ) {
			for(int i=1; i<colNames1.count(); i++)
				if(colNames1.at(i) == "encoder")
					encoderEndpoint = int(lp.at(i).toDouble());
		}
		// event id 2.  If the line starts with "# 2," and there are the correct number of columns:
		if(line.startsWith("# 2,") && (lp = line.split(',')).count() == colNames2.count() ) {
			// see if we recognize any of the translated column names as useful:
			for(int i=1; i<colNames2.count(); i++) {
				if(colNames2.at(i) == "time")
					datetime = QDateTime::fromTime_t(uint(lp.at(i).toDouble()));
				if(colNames2.at(i) == "grating")
					grating = lp.at(i);
				if(colNames2.at(i) == "encoder"){
					hasEncoderInfo = true;
					encoderStartPoint = int(lp.at(i).toDouble());
				}
			}
		}
	}

	QString scalerFile = "";
	QFileInfo scalerFileInfo;
	if(colNames1.contains("scaler_fileOffset")){
		foreach(QString afp, scan->additionalFilePaths())
			if(afp.contains("_spectra.dat")){
				scalerFile = afp;
				scalerFileInfo.setFile(scalerFile);
				if(scalerFileInfo.isRelative())
					scalerFileInfo.setFile(userDataFolder + "/" + scalerFile);
			}
		if(scalerFile != ""){
			//QFile sf(scalerFile);
			QFile sf(scalerFileInfo.filePath());
			if(!sf.open(QIODevice::ReadOnly)) {
				errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2010FASTFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE, "SGM2010FastFileLoader parse error while loading scan data from file. Could not open spectra.dat file."));
				return false; //spectra.dat file couldn't be opened
			}
			QTextStream sfs(&sf);
			QTextStream sfls;
			int numScalerReadings;
			int scalerVal;
			QString sfl = sfs.readAll().replace(',', ' ');
			sfls.setString(&sfl, QIODevice::ReadOnly);
			sfls >> numScalerReadings;

			if(numScalerReadings == 10000){
				defaultUserVisibleColumns_ << "Aux1";
				defaultUserVisibleColumns_ << "Aux2";
				defaultUserVisibleColumns_ << "Aux3";
				defaultUserVisibleColumns_ << "Aux4";

				scan->rawData()->addMeasurement(AMMeasurementInfo("Aux1", "Aux1"));
				scan->rawData()->addMeasurement(AMMeasurementInfo("Aux2", "Aux2"));
				scan->rawData()->addMeasurement(AMMeasurementInfo("Aux3", "Aux3"));
				scan->rawData()->addMeasurement(AMMeasurementInfo("Aux4", "Aux4"));
			}

			if(numScalerReadings == 4000){
				for(int x = 0; x < numScalerReadings; x++){
					if(x%4 == 0){
						scan->rawData()->beginInsertRows(1, -1);
						scan->rawData()->setAxisValue(0, x/4, x/4);
					}
					sfls >> scalerVal;
					scan->rawData()->setValue(AMnDIndex(x/4), x%4, AMnDIndex(), scalerVal);
					if(x%4 == 3 || x == numScalerReadings-1)
						scan->rawData()->endInsertRows();
				}
			}
			else if(numScalerReadings == 6000){
				//int encoderStartPoint = 0;
				int encoderReading = 0;
				double energyFbk = 0.0;

				double spacingParam;
				double c1Param;
				double c2Param;
				double sParam;
				double thetaParam;
				if(scan->scanConfiguration()){
					spacingParam = scan->scanConfiguration()->property("spacingParameter").toDouble();
					c1Param = scan->scanConfiguration()->property("c1Parameter").toDouble();
					c2Param = scan->scanConfiguration()->property("c2Parameter").toDouble();
					sParam = scan->scanConfiguration()->property("sParameter").toDouble();
					thetaParam = scan->scanConfiguration()->property("thetaParameter").toDouble();
				}
				else{
					spacingParam = 1.68923e-06;
					c1Param = 2.45477e-05;
					c2Param = -1.59392;
					sParam = 509.468;
					thetaParam = 3.05575;
				}

				QList<double> readings;

				if(!hasEncoderInfo){
					encoderStartPoint = encoderEndpoint;
					for(int x = 0; x < numScalerReadings; x++){
						sfls >> scalerVal;
						//if( (x%6 == 4) && (scalerVal < 40) )
						if( x%6 == 4 )
							encoderStartPoint += scalerVal;
						//if( (x%6 == 5) && (scalerVal < 40) )
						if( x%6 == 5 )
							encoderStartPoint -= scalerVal;
					}
				}
				encoderReading = encoderStartPoint;
				sfls.setString(&sfl, QIODevice::ReadOnly);
				sfls >> scalerVal;
				for(int x = 0; x < numScalerReadings; x++){
					if(x%6 == 0)
						readings.clear();
					sfls >> scalerVal;
					if( x%6 == 0 || x%6 == 1 || x%6 == 2 || x%6 == 3 ){
						//if( x%6 == 0 || x%6 == 1 || x%6 == 4 || x%6 == 5 ){
						readings.append(scalerVal);
					}
					//if( (x%6 == 4) && (scalerVal < 40) )
					if( x%6 == 4 )
						encoderReading -= scalerVal;
					//if( (x%6 == 5) && (scalerVal < 40) )
					if ( x%6 == 5 )
						encoderReading += scalerVal;
					if( x%6 == 5 ){
						//energyFbk = (1.0e-9*1239.842*511.292)/(2*9.16358e-7*2.46204e-5*-1.59047*(double)encoderReading*cos(3.05478/2));
						energyFbk = (1.0e-9*1239.842*sParam)/(2*spacingParam*c1Param*c2Param*(double)encoderReading*cos(thetaParam/2));
						//if( ( (readings.at(0) > 200) && (scan->rawData()->scanSize(0) == 0) ) || ( (scan->rawData()->scanSize(0) > 0) && (fabs(energyFbk - (double)scan->rawData()->axisValue(0, scan->rawData()->scanSize(0)-1)) > 0.001) ) ){
						scan->rawData()->beginInsertRows(1, -1);
						scan->rawData()->setAxisValue(0, scan->rawData()->scanSize(0)-1, energyFbk);
						//scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 0, AMnDIndex(), readings.at(0));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 0, AMnDIndex(), std::max(readings.at(0), 1.0));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 1, AMnDIndex(), readings.at(1));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 2, AMnDIndex(), readings.at(2));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 3, AMnDIndex(), readings.at(3));
						scan->rawData()->endInsertRows();
						//}
					}
				}

			}
			else if(numScalerReadings == 10000){
				int encoderReading = 0;
				double energyFbk = 0.0;

				double spacingParam;
				double c1Param;
				double c2Param;
				double sParam;
				double thetaParam;
				if(scan->scanConfiguration()){
					spacingParam = scan->scanConfiguration()->property("spacingParameter").toDouble();
					c1Param = scan->scanConfiguration()->property("c1Parameter").toDouble();
					c2Param = scan->scanConfiguration()->property("c2Parameter").toDouble();
					sParam = scan->scanConfiguration()->property("sParameter").toDouble();
					thetaParam = scan->scanConfiguration()->property("thetaParameter").toDouble();
				}
				else{
					spacingParam = 1.68923e-06;
					c1Param = 2.45477e-05;
					c2Param = -1.59392;
					sParam = 509.468;
					thetaParam = 3.05575;
				}

				QList<double> readings;

				if(!hasEncoderInfo){
					encoderStartPoint = encoderEndpoint;
					for(int x = 0; x < numScalerReadings; x++){
						sfls >> scalerVal;

						if( x%10 == 4 )
							encoderStartPoint += scalerVal;
						if( x%10 == 5 )
							encoderStartPoint -= scalerVal;
					}
				}
				encoderReading = encoderStartPoint;
				sfls.setString(&sfl, QIODevice::ReadOnly);
				sfls >> scalerVal;
				for(int x = 0; x < numScalerReadings; x++){
					if(x%10 == 0)
						readings.clear();
					sfls >> scalerVal;
					if( x%10 == 0 || x%10 == 1 || x%10 == 2 || x%10 == 3 || x%10 == 6 || x%10 == 7 || x%10 == 8 || x%10 == 9){
						readings.append(scalerVal);
					}
					if( x%10 == 4 )
						encoderReading -= scalerVal;
					if ( x%10 == 5 )
						encoderReading += scalerVal;
					if( x%10 == 9 ){
						energyFbk = (1.0e-9*1239.842*sParam)/(2*spacingParam*c1Param*c2Param*(double)encoderReading*cos(thetaParam/2));
						scan->rawData()->beginInsertRows(1, -1);
						scan->rawData()->setAxisValue(0, scan->rawData()->scanSize(0)-1, energyFbk);
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 0, AMnDIndex(), std::max(readings.at(0), 1.0));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 1, AMnDIndex(), readings.at(1));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 2, AMnDIndex(), readings.at(2));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 3, AMnDIndex(), readings.at(3));

						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 4, AMnDIndex(), readings.at(4));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 5, AMnDIndex(), readings.at(5));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 6, AMnDIndex(), readings.at(6));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 7, AMnDIndex(), readings.at(7));

						scan->rawData()->endInsertRows();
					}
				}
			}
		}
		else{
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2010FASTFILELOADERPLUGIN_MISSING_SPECTRA_FILE, "SGM2010FastFileLoader parse error while loading scan data from file. I couldn't find the spectra.dat file."));
			return false;	// bad format; no spectra.dat file
		}
	}
	else{
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2010FASTFILELOADERPLUGIN_BAD_FORMAT_NO_OFFSET_COLUMN, "SGM2010FastFileLoader parse error while loading scan data from file. I couldn't find the spectrum offset column."));
		return false;	// bad format; no spectrum offset column
	}


	/// Not supposed to create the raw data sources.  Do an integrity check on the pre-existing data sources instead... If there's a raw data source, but it's pointing to a non-existent measurement in the data store, that's a problem. Remove it.  \todo Is there any way to incorporate this at a higher level, so that import-writers don't need to bother?
	for(int i=0; i<scan->rawDataSources()->count(); i++) {
		if(scan->rawDataSources()->at(i)->measurementId() >= scan->rawData()->measurementCount()) {
			errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Debug, SGM2010FASTFILELOADERPLUGIN_DATA_COLUMN_MISMATCH, QString("SGM2010FastFileLoader: The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
			scan->deleteRawDataSource(i);
		}
	}

	return true;
}

bool SGM2010FastFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "sgm2010Fast");
}

Q_EXPORT_PLUGIN2(SGM2010FastFileLoaderFactory, SGM2010FastFileLoaderFactory)

