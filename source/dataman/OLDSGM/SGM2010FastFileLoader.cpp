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


#include "SGM2010FastFileLoader.h"
#include <QDir>

AMBiHash<QString, QString> SGM2010FastFileLoader::columns2pvNames_;

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include "dataman/AMFastScan.h"
#include "acquaman/AMScanConfiguration.h"
#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM1DRunningAverageFilterAB.h"
#include "analysis/SGM/SGM1DFastScanFilterAB.h"

#include <algorithm>

SGM2010FastFileLoader::SGM2010FastFileLoader(AMFastScan *scan) : AMAbstractFileLoader(scan)
{
	if(columns2pvNames_.count() == 0) {
		columns2pvNames_.set("eV", "BL1611-ID-1:Energy");
		columns2pvNames_.set("scaler_fileOffset", "BL1611-ID-1:mcs:scan");
		columns2pvNames_.set("time", "Absolute-Time-Stamp");
		columns2pvNames_.set("grating", "SG16114I1001:choice");
		columns2pvNames_.set("encoder", "SMTR16114I1002:enc:fbk");
	}

	defaultUserVisibleColumns_ << "TEY";
	defaultUserVisibleColumns_ << "TFY";
	defaultUserVisibleColumns_ << "I0";
	defaultUserVisibleColumns_ << "Photodiode";
}

SGM2010FastFileLoader::~SGM2010FastFileLoader(){}

/// load raw data from the SGM fast scan _spectra.dat file format into a scan's data tree.  If \c extractMetaData is set to true, this will also set the 'notes' and 'dateTime' meta-data fields.  If \c createChannels is set to true, it will create some default channels based on the data columns.
bool SGM2010FastFileLoader::loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks) {

	// not initialized to have a scan target, or scan target is not an AMFastScan...
	AMFastScan* scan = qobject_cast<AMFastScan*>(scan_);
	if(!scan)
		return false;

	// Clear the old scan axes to ensure we don't have any extras.
	scan->clearRawDataCompletely();
	scan->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

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
	QFile f(filepath);
	if(!f.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2010FastFileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	if(setMetaData) {
		// Start reading the file. look for comment line.
		while( !fs.atEnd() && fs.readLine() != QString("# COMMENT"))
			;
		if(fs.atEnd()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -2, "SGM2010FastFileLoader parse error while loading scan data from file. Could not find the comment."));
			fs.seek(0);
		}
		else {

			// read the comment
			line = fs.readLine();
			// chop off the "# "
			comments = line.remove(0,1).trimmed();
		}
	}

	// find out what columns exist. Looking for line starting with '#(1) '
	// find out what information we've got in event ID 1
	line.clear();
	while(!fs.atEnd() && !line.startsWith("#(1) "))
		line = fs.readLine();
	if(fs.atEnd()) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "SGM2010FastFileLoader parse error while loading scan data from file. Missing #(1) event line."));
		return false;	// bad format. Missing the #1 event header
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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "SGM2010FastFileLoader parse error while loading scan data from file. Missing #(2) event line."));
		return false;	// bad format. Missing the #2 event header
	}
	colNames2 = line.split(QChar(' '));
	// the first column is not a column name, it's just the event description header ("#(1)")
	colNames2.removeFirst();
	for(int i=0; i<colNames2.count(); i++)
		pv2columnName(colNames2[i]);

	// ensure that we have the basic "eV" column
	int eVIndex = colNames1.indexOf("eV");
	if(eVIndex < 0) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "SGM2010FastFileLoader parse error while loading scan data from file. I couldn't find the energy (eV) column."));
		return false;	// bad format. No primary column

	}

	// clear the existing raw data (and raw data sources, if we're supposed to)
	if(setRawDataSources)
		scan->clearRawDataPointsAndMeasurementsAndDataSources();
	else
		scan->clearRawDataPointsAndMeasurements();

	scan->rawData()->addMeasurement(AMMeasurementInfo("TEY", "TEY"));
	scan->rawData()->addMeasurement(AMMeasurementInfo("TFY", "TFY"));
	scan->rawData()->addMeasurement(AMMeasurementInfo("I0", "I0"));
	scan->rawData()->addMeasurement(AMMeasurementInfo("Photodiode", "Photodiode"));

	while(!fs.atEnd()) {
		line = fs.readLine();
		if(line.startsWith("1,") && (lp = line.split(',')).count() == colNames1.count() ) {
			for(int i=1; i<colNames1.count(); i++) {
				if(colNames1.at(i) == "encoder"){
					encoderEndpoint = int(lp.at(i).toDouble());
				}
			}
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
	//if(scan->rawData()->idOfMeasurement("scaler_fileOffset") >= 0){
	if(colNames1.contains("scaler_fileOffset")){
		foreach(QString afp, scan->additionalFilePaths())
			if(afp.contains("_spectra.dat")){
				scalerFile = afp;
				scalerFileInfo.setFile(scalerFile);
				if(scalerFileInfo.isRelative())
					scalerFileInfo.setFile(AMUserSettings::userDataFolder + "/" + scalerFile);
			}
		if(scalerFile != ""){
			QFile sf(scalerFileInfo.filePath());
			if(!sf.open(QIODevice::ReadOnly)) {
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2010FastFileLoader parse error while loading scan data from file. Could not open spectra.dat file."));
				return false; //spectra.dat file couldn't be opened
			}
			QTextStream sfs(&sf);
			QTextStream sfls;
			int numScalerReadings;
			int scalerVal;
			QString sfl = sfs.readAll().replace(',', ' ');
			sfls.setString(&sfl, QIODevice::ReadOnly);
			sfls >> numScalerReadings;
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
						readings.append(scalerVal);
					}
					if( x%6 == 4 )
						encoderReading -= scalerVal;
					if ( x%6 == 5 )
						encoderReading += scalerVal;
					if( x%6 == 5 ){
						energyFbk = (1.0e-9*1239.842*sParam)/(2*spacingParam*c1Param*c2Param*(double)encoderReading*cos(thetaParam/2));
						scan->rawData()->beginInsertRows(1, -1);
						scan->rawData()->setAxisValue(0, scan->rawData()->scanSize(0)-1, energyFbk);
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 0, AMnDIndex(), std::max(readings.at(0), 1.0));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 1, AMnDIndex(), readings.at(1));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 2, AMnDIndex(), readings.at(2));
						scan->rawData()->setValue(AMnDIndex(scan->rawData()->scanSize(0)-1), 3, AMnDIndex(), readings.at(3));
						scan->rawData()->endInsertRows();
					}
				}
			}
		}
		else{
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "SGM2010FastFileLoader parse error while loading scan data from file. I couldn't find the spectra.dat file."));
			return false;	// bad format. No spectra.dat file
		}
	}
	else{
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "SGM2010FastFileLoader parse error while loading scan data from file. I couldn't find the spectrum offset column."));
		return false;	// bad format. No spectrum offset column
	}

	if(setMetaData) {
		scan->setNotes(QString("Grating: %1\nComments:\n%2").arg(grating).arg(comments));	/// \todo Move this from notes to the scan's scanInitialConditions().
		scan->setDateTime(datetime);
	}

	// If we need to create the raw data sources...
	if(setRawDataSources) {
		// expose the raw data that might be useful to the users
		foreach(QString visibleColumn, defaultUserVisibleColumns_) {
			int measurementId = scan->rawData()->idOfMeasurement(visibleColumn);
			if(measurementId >= 0)
				scan->addRawDataSource(new AMRawDataSource(scan->rawData(), measurementId));
		}
	}

	/// Not supposed to create the raw data sources.  Do an integrity check on the pre-existing data sources instead... If there's a raw data source, but it's pointing to a non-existent measurement in the data store, that's a problem. Remove it.  \todo Is there any way to incorporate this at a higher level, so that import-writers don't need to bother?
	else {
		for(int i=0; i<scan->rawDataSources()->count(); i++) {
			if(scan->rawDataSources()->at(i)->measurementId() >= scan->rawData()->measurementCount()) {
				AMErrorMon::report(AMErrorReport(scan, AMErrorReport::Debug, -97, QString("SGM2010FastFileLoader: The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
				scan->deleteRawDataSource(i);
			}
		}
	}

	// If the scan doesn't have any channels yet, it would be helpful to create some.
	if(createDefaultAnalysisBlocks) {

		QList<AMDataSource*> raw1DDataSources;
		for(int i=0; i<scan->rawDataSources()->count(); i++)
			if(scan->rawDataSources()->at(i)->rank() == 1)
				raw1DDataSources << scan->rawDataSources()->at(i);

		int rawTeyIndex = scan->rawDataSources()->indexOfKey("tey");
		int rawTfyIndex = scan->rawDataSources()->indexOfKey("tfy");
		int rawI0Index = scan->rawDataSources()->indexOfKey("I0");

		if(rawTeyIndex != -1 && rawI0Index != -1) {
			AM1DExpressionAB* teyChannel = new AM1DExpressionAB("tey_n");
			teyChannel->setDescription("Normalized TEY");
			teyChannel->setInputDataSources(raw1DDataSources);
			teyChannel->setExpression("tey/I0");

			scan->addAnalyzedDataSource(teyChannel);
		}

		if(rawTfyIndex != -1 && rawI0Index != -1) {
			AM1DExpressionAB* tfyChannel = new AM1DExpressionAB("tfy_n");
			tfyChannel->setDescription("Normalized TFY");
			tfyChannel->setInputDataSources(raw1DDataSources);
			tfyChannel->setExpression("-tfy/I0");

			scan->addAnalyzedDataSource(tfyChannel);
		}
	}

	return true;
}

bool SGM2010FastFileLoader::saveToFile(const QString &filepath){
	Q_UNUSED(filepath)
	return false;
}
