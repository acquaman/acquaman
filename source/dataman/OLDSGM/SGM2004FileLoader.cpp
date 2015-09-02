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


#include "SGM2004FileLoader.h"
#include <QDir>

AMBiHash<QString, QString> SGM2004FileLoader::columns2pvNames_;

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QDateTime>
#include "dataman/AMXASScan.h"
#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

SGM2004FileLoader::SGM2004FileLoader(AMXASScan* scan) : AMAbstractFileLoader(scan)
{
	// this static storage can be shared across all instances, but if we're the first, need to populate it.
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

		columns2pvNames_.set("sdd_ROI_0", "MCA1611-01:ROI:0");
		columns2pvNames_.set("sdd_ROI_1", "MCA1611-01:ROI:1");
		columns2pvNames_.set("sdd_ROI_2", "MCA1611-01:ROI:2");
		columns2pvNames_.set("sdd_ROI_3", "MCA1611-01:ROI:3");
		columns2pvNames_.set("sdd_deadtime", "MCA1611-01:DeadFraction");
		columns2pvNames_.set("sdd_fileOffset", "MCA1611-01:GetChannels");
	}


	defaultUserVisibleColumns_ << "TEY";
	defaultUserVisibleColumns_ << "TFY";
	defaultUserVisibleColumns_ << "I0";
	defaultUserVisibleColumns_ << "Photodiode";
	defaultUserVisibleColumns_ << "EnergyFeedback";
	defaultUserVisibleColumns_ << "ringCurrent";
	defaultUserVisibleColumns_ << "SDD";
}

SGM2004FileLoader::~SGM2004FileLoader(){}

/// load raw data from the SGM legacy file format into a scan's data tree.  If \c extractMetaData is set to true, this will also set the 'notes' and 'dateTime' meta-data fields.  If \c createChannels is set to true, it will create some default channels based on the data columns.
bool SGM2004FileLoader::loadFromFile(const QString& filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks) {

	// not initialized to have a scan target, or scan target is not an AMXASScan...
	AMXASScan* scan = qobject_cast<AMXASScan*>(scan_);
	if(!scan)
		return false;

	// Clear the old scan axes to ensure we don't have any extras.
	scan->clearRawDataCompletely();
	scan->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

	// information about the scan we hope to locate:
	QString comments;
	QDateTime datetime;
	double integrationTime = 0;
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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "SGM2004FileLoader parse error while loading scan data from file. Missing #(2) event line."));
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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "SGM2004FileLoader parse error while loading scan data from file. I couldn't find the energy (eV) column."));
		return false;	// bad format. No primary column

	}


	// clear the existing raw data (and raw data sources, if we're supposed to)
	if(setRawDataSources)
		scan->clearRawDataPointsAndMeasurementsAndDataSources();
	else
		scan->clearRawDataPointsAndMeasurements();


	// There is a rawData scan axis called "eV" created in the constructor.  AMAxisInfo("eV", 0, "Incident Energy", "eV")
	/// \todo What if there isn't? Should we check, and create the axis if none exist? What if there's more than one scan axis? Can't remove from AMDataStore... [The rest of this code assumes a single scan axis]


	bool postSddFileOffset = false;
	QString spectraFile = "";
	// add scalar (0D) measurements to the raw data store, for each data column.  If setRawDataSources is true, also add raw data sources to the scan, which expose this data.
	foreach(QString colName, colNames1) {
		if(colName != "eV" && colName != "Event-ID") {
			if(colName == "sdd_fileOffset"){
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
					}
				}
			}
			else if(scan->rawData()->addMeasurement(AMMeasurementInfo(colName, colName))){
				/// \todo Throw an errorMon out?
			}
		}
	}
	if(postSddFileOffset){
		scan->rawData()->addMeasurement(AMMeasurementInfo("sdd_fileOffset", "sdd_fileOffset"));
	}
	int sddOffsetIndex = colNames1.indexOf("sdd_fileOffset");

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
					scan->rawData()->setValue(eVAxisIndex, scan->rawData()->idOfMeasurement("sdd_fileOffset"), AMnDIndex(), lp.at(i).toDouble());
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
			spectraFileInfo.setFile(AMUserSettings::userDataFolder + "/" + spectraFile);

		QFile sf(spectraFileInfo.filePath());
		if(!sf.open(QIODevice::ReadOnly)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2004FileLoader parse error while loading scan data from file. Missing SDD spectra file."));
			return false;
		}

		int startByte = 0, endByte = 0;
		int specCounter = 0;
		int scanSize = scan->rawData()->scanSize(0);
		int fileOffsetMeasurementId = scan->rawData()->idOfMeasurement("sdd_fileOffset");
		int sddMeasurementId = scan->rawData()->idOfMeasurement("SDD");
		int sddSize = scan->rawData()->measurementAt(sddMeasurementId).size(0);
		int* specValues = new int[sddSize];

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
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, QString("SGM2004FileLoader found corrupted data in the SDD spectra file '%1' on row %2. There should be %3 elements in the spectra, but we only found %4").arg(spectraFile).arg(x).arg(sddSize).arg(specCounter)));
			}

			specCounter = 0;
		}
		delete specValues;
	}


	if(setMetaData) {
		scan->setNotes(QString("Grating: %1\nIntegration Time: %2\nComments:\n%3").arg(grating).arg(integrationTime).arg(comments));	/// \todo Move this from notes to the scan's scanInitialConditions().
		scan->setDateTime(datetime);
	}


	// If we need to create the raw data sources...
	if(setRawDataSources){
		for(int x = 0; x < scan->rawData()->measurementCount(); x++){
			if(defaultUserVisibleColumns_.contains(scan->rawData()->measurementAt(x).name))
				scan->addRawDataSource(new AMRawDataSource(scan->rawData(), x));
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

		int rawTeyIndex = scan->rawDataSources()->indexOfKey("TEY");
		int rawTfyIndex = scan->rawDataSources()->indexOfKey("TFY");
		int rawI0Index = scan->rawDataSources()->indexOfKey("I0");

		if(rawTeyIndex != -1 && rawI0Index != -1) {
			AM1DExpressionAB* teyChannel = new AM1DExpressionAB("TEYNorm");
			teyChannel->setDescription("Normalized TEY");
			teyChannel->setInputDataSources(raw1DDataSources);
			teyChannel->setExpression("TEY/I0");

			scan->addAnalyzedDataSource(teyChannel);
		}

		if(rawTfyIndex != -1 && rawI0Index != -1) {
			AM1DExpressionAB* tfyChannel = new AM1DExpressionAB("TFYNorm");
			tfyChannel->setDescription("Normalized TFY");
			tfyChannel->setInputDataSources(raw1DDataSources);
			tfyChannel->setExpression("-TFY/I0");

			scan->addAnalyzedDataSource(tfyChannel);
		}


		int rawSddIndex = scan->rawDataSources()->indexOfKey("SDD");
		if(rawSddIndex != -1) {
			AMRawDataSource* sddRaw = scan->rawDataSources()->at(rawSddIndex);
			AM2DSummingAB* sddSum = new AM2DSummingAB("PFY");
			QList<AMDataSource*> sddSumSource;
			sddSumSource << sddRaw;
			sddSum->setInputDataSources(sddSumSource);
			sddSum->setSumAxis(1);
			sddSum->setSumRangeMax(sddRaw->size(1)-1);

			scan->addAnalyzedDataSource(sddSum);
		}
	}
	return true;
}
