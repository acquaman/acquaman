/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "SGM2004FileLoader.h"
#include <QDir>

AMBiHash<QString, QString> SGM2004FileLoader::columns2pvNames_;

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDateTime>
#include "dataman/AMXASScan.h"
#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

#include <QDebug>


SGM2004FileLoader::SGM2004FileLoader(AMXASScan* scan) : AMAbstractFileLoader(scan)
{
	// this static storage can be shared across all instances, but if we're the first, need to populate it.
	if(columns2pvNames_.count() == 0) {
		columns2pvNames_.set("eV", "BL1611-ID-1:Energy");
		columns2pvNames_.set("ringCurrent", "PCT1402-01:mA:fbk");
		columns2pvNames_.set("I0_2", "A1611-4-13:A:fbk");
		columns2pvNames_.set("I0", "A1611-4-14:A:fbk");
		columns2pvNames_.set("tey", "A1611-4-15:A:fbk");
		columns2pvNames_.set("tfy", "A1611-4-16:A:fbk");
		columns2pvNames_.set("eV_fbk", "BL1611-ID-1:Energy:fbk");
		columns2pvNames_.set("pressure", "TCGC1611-423:pressure:fbk");

		columns2pvNames_.set("integrationTime", "A1611I1:cont_interval");
		columns2pvNames_.set("grating", "SG16114I1001:choice");
		columns2pvNames_.set("time", "Absolute-Time-Stamp");

		columns2pvNames_.set("sdd_ROI_0", "MCA1611-01:ROI:0");
		columns2pvNames_.set("sdd_ROI_1", "MCA1611-01:ROI:1");
		columns2pvNames_.set("sdd_ROI_2", "MCA1611-01:ROI:2");
		columns2pvNames_.set("sdd_ROI_3", "MCA1611-01:ROI:3");
		columns2pvNames_.set("sdd_deadtime", "MCA1611-01:DeadFraction");
		columns2pvNames_.set("sdd_fileOffset", "MCA1611-01:GetChannels");
	}


	defaultUserVisibleColumns_ << "tey";
	defaultUserVisibleColumns_ << "tfy";
	defaultUserVisibleColumns_ << "I0";
	defaultUserVisibleColumns_ << "I0_2";
	defaultUserVisibleColumns_ << "eV_fbk";
	defaultUserVisibleColumns_ << "ringCurrent";
	defaultUserVisibleColumns_ << "sdd";
}

/// load raw data from the SGM legacy file format into a scan's data tree.  If \c extractMetaData is set to true, this will also set the 'notes' and 'dateTime' meta-data fields.  If \c createChannels is set to true, it will create some default channels based on the data columns.
bool SGM2004FileLoader::loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks) {

	// not initialized to have a scan target, or scan target is not an AMXASScan...
	AMXASScan* scan = qobject_cast<AMXASScan*>(scan_);
	if(!scan)
		return false;

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
	QFile f(filepath);
	if(!f.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2004FileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	if(setMetaData) {
		// Start reading the file. look for comment line.
		while( !fs.atEnd() && fs.readLine() != QString("# COMMENT"))
			;
		if(fs.atEnd()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, -2, "SGM2004FileLoader parse error while loading scan data from file. Could not find the comment."));
			fs.seek(0);
			// return false;	// bad format; missing the comment string
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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "SGM2004FileLoader parse error while loading scan data from file. Missing #(1) event line."));
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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "SGM2004FileLoader parse error while loading scan data from file. Missing #(2) event line."));
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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "SGM2004FileLoader parse error while loading scan data from file. I couldn't find the energy (eV) column."));
		return false;	// bad format; no primary column

	}


	// clear the existing raw data (and raw data sources, if we're supposed to)
	if(setRawDataSources)
		scan->clearRawDataPointsAndMeasurementsAndDataSources();
	else
		scan->clearRawDataPointsAndMeasurements();


	// There is a rawData scan axis called "eV" created in the constructor.  AMAxisInfo("eV", 0, "Incident Energy", "eV")
	/// \todo What if there isn't? Should we check, and create the axis if none exist? What if there's more than one scan axis? Can't remove from AMDataStore... [The rest of this code assumes a single scan axis]


	// add scalar (0D) measurements to the raw data store, for each data column.  If setRawDataSources is true, also add raw data sources to the scan, which expose this data.
	foreach(QString colName, colNames1) {
		if(colName != "eV" && colName != "Event-ID") {
			scan->rawData()->addMeasurement(AMMeasurementInfo(colName, colName));	/// \todo nice descriptions for the common column names; not just 'tey' or 'tfy'.
		}
	}
	QString spectraFile = "";
	if(scan->rawData()->idOfMeasurement("sdd_fileOffset") >= 0){
		foreach(QString afp, scan->additionalFilePaths())
			if(afp.contains("_spectra.dat"))
				spectraFile = afp;
		if(spectraFile != ""){
			AMAxisInfo sddEVAxisInfo("energy", 1024, "SDD Energy", "eV");
			QList<AMAxisInfo> sddAxes;
			sddAxes << sddEVAxisInfo;
			AMMeasurementInfo sddInfo("sdd", "Silicon Drift Detector", "counts", sddAxes);
			scan->rawData()->addMeasurement(sddInfo);
		}
	}

	// read all the data. Add to data columns or scan properties depending on the event-ID.

	int eVAxisIndex = 0;	// counter, incremented for every data point along the scan (eV) axis.
	while(!fs.atEnd()) {

		line = fs.readLine();

		// event id 1.  If the line starts with "1," and there are the correct number of columns:
		if(line.startsWith("1,") && (lp = line.split(',')).count() == colNames1.count() ) {

			scan->rawData()->beginInsertRows(0);
			scan->rawData()->setAxisValue(0, eVAxisIndex, lp.at(eVIndex).toDouble()); // insert eV

			// add data from all columns (but ignore the first (Event-ID) and the eV column)
			int measurementId = 0;
			for(int i=1; i<colNames1.count(); i++) {
				if(i!=eVIndex) {
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
					datetime = QDateTime::fromTime_t(lp.at(i).toDouble());
				if(colNames2.at(i) == "grating")
					grating = lp.at(i);
				if(colNames2.at(i) == "integrationTime")
					integrationTime = lp.at(i).toDouble();
			}
		}
	}


	if(spectraFile != ""){
		QFile sf(spectraFile);
		if(!sf.open(QIODevice::ReadOnly)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2004FileLoader parse error while loading scan data from file. Missing spectra file."));
			return false;
		}
		QTextStream sfs(&sf);
		QTextStream sfls;
		int startByte, endByte;
		int specVal;
		int specCounter = 0;
		for(int x = 0; x < scan->rawData()->scanSize(0); x++){
			if(x == scan->rawData()->scanSize(0)-1){
				endByte += endByte-startByte; //Assumes the last two are the same size
				startByte = scan->rawData()->value(AMnDIndex(x), scan->rawData()->idOfMeasurement("sdd_fileOffset"), AMnDIndex());
			}
			else{
				startByte = scan->rawData()->value(AMnDIndex(x), scan->rawData()->idOfMeasurement("sdd_fileOffset"), AMnDIndex());
				endByte = scan->rawData()->value(AMnDIndex(x+1), scan->rawData()->idOfMeasurement("sdd_fileOffset"), AMnDIndex());
			}
			//sfs.seek( (qint64)((int)(scan->rawData()->value(AMnDIndex(x), scan->rawData()->idOfMeasurement("sdd_fileOffset"), AMnDIndex()))) );
			sfs.seek(startByte);
			QString sfl = sfs.read( (qint64)(endByte-startByte) ).remove(',');
			sfls.setString(&sfl, QIODevice::ReadOnly);

			while(!sfls.atEnd()){
				sfls >> specVal;
				scan->rawData()->setValue(AMnDIndex(x), scan->rawData()->idOfMeasurement("sdd"), AMnDIndex(specCounter), specVal);
				specCounter++;
			}

			specCounter = 0;
		}
	}


	if(setMetaData) {
		scan->setNotes(QString("Grating: %1\nIntegration Time: %2\nComments:\n%3").arg(grating).arg(integrationTime).arg(comments));	/// \todo Move this from notes to the scan's scanInitialConditions().
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
				AMErrorMon::report(AMErrorReport(scan, AMErrorReport::Debug, -97, QString("The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
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


		int rawSddIndex = scan->rawDataSources()->indexOfKey("sdd");
		if(rawSddIndex != -1) {
			AMRawDataSource* sddRaw = scan->rawDataSources()->at(rawSddIndex);
			AM2DSummingAB* sddSum = new AM2DSummingAB("sddSummed");
			QList<AMDataSource*> sddSumSource;
			sddSumSource << sddRaw;
			sddSum->setInputDataSources(sddSumSource);
			sddSum->setSumAxis(1);
			sddSum->setSumRangeMax(sddRaw->size(1)-1);

			scan->addAnalyzedDataSource(sddSum);
		}
	}

	/// scan->onDataChanged(); \todo Is this still used? What does it mean?


	// Debugging only... What's here?

	/*
	qDebug() << "========= Breakdown for scan named: " << scan->name() << "==============";
	qDebug() << "Scan axes\n===========================";
	for(int i=0; i<scan->rawData()->scanAxesCount(); i++)
		qDebug() << scan->rawData()->scanAxisAt(i).name << ": " << scan->rawData()->scanAxisAt(i).size << "points.";

	qDebug() << "Measurements\n===========================";
	for(int i=0; i<scan->rawData()->measurementCount(); i++)
		qDebug() << scan->rawData()->measurementAt(i).name;

	qDebug() << "Raw Data\n===========================";
	for(int i=0; i<scan->rawDataSources()->count(); i++) {
		AMRawDataSource* rds = scan->rawDataSources()->at(i);
		qDebug() << rds->name() << ":" << rds->description() << ": isValid is " << rds->isValid() << ": numPoints is " << rds->size(0);
	}
	qDebug() << "Analyzed Data\n===========================";
	for(int i=0; i<scan->analyzedDataSources()->count(); i++) {
		AMAnalysisBlock* rds = scan->analyzedDataSources()->at(i);
		qDebug() << rds->name() << ":" << rds->description() << ": isValid is " << rds->isValid() << ": numPoints is " << rds->size(0);
	}

//	qDebug() << "Raw data source - SDD - Axis 0\n======================";
//	QString axisValues;
//	AMRawDataSource* sddRds = scan->rawDataSources()->at(scan->rawDataSources()->count()-1);
//	for(int i=0; i<sddRds->size(0); i++)
//		axisValues.append(QString("%1, ").arg((double)sddRds->axisValue(0, i)));
//	qDebug() << axisValues;

//	qDebug() << "Raw data source - SDD - Axis 1\n======================";
//	axisValues.clear();
//	for(int i=0; i<sddRds->size(1); i++)
//		axisValues.append(QString("%1, ").arg((double)sddRds->axisValue(1, i)));
//	qDebug() << axisValues;


	QString row;
	qDebug() << "First measurement, raw data axis value:";
	for(int i=0; i<scan->rawData()->scanSize(0); i++)
		row.append(QString("%1, ").arg((double)scan->rawData()->axisValue(0,i)));
	qDebug() << "   " << row;

	qDebug() << "From raw data source:";
	row.clear();
	AMRawDataSource* rds = scan->rawDataSources()->at(0);
	for(int i=0; i<rds->size(0); i++)
		row.append(QString("%1, ").arg((double)rds->axisValue(0,i)));
	qDebug() << "    " << row;
*/

	return true;
}
