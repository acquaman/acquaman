#include "SGM2013XASFileLoaderPlugin.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

#include "dataman/AMScan.h"

bool SGM2013XASFileLoaderPlugin::accepts(AMScan *scan){
	qDebug() << "SGM2013XAS trying to accept " << scan->fileFormat();
	if(scan->fileFormat() == "sgm2013XAS")
		return true;
	return false;
}

bool SGM2013XASFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder){
	if(!scan)
		return false;

	// Clear the old scan axes to ensure we don't have any extras.
	scan->clearRawDataCompletely();
	scan->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	// open the file:
	QFile f(sourceFileInfo.filePath());
	if(!f.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2013XASFileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	// used in parsing the data file
	QString line;
	QStringList lp;
	int index, rank;
	QString infoName, infoDescription, infoUnits;
	QString axisName, axisDescription, axisUnits;
	int axisSize;
	int dataIndex;
	double dataValue;
	int insertionIndex = 0;
	bool informationSection = false;
	bool finishedHeader = false;
	while(!fs.atEnd()){
		if(!finishedHeader)
			line = fs.readLine();
		if(line == "Start Info")
			informationSection = true;
		else if(line == "End Info"){
			informationSection = false;
			finishedHeader = true;
			line = "";
		}
		else if(informationSection){
			lp = line.split("|%|%|");
			index = lp.at(0).toInt();
			if(index >= 0){
				rank = lp.at(1).toInt();
				infoName = lp.at(2);
				infoDescription = lp.at(3);
				infoUnits = lp.at(4);
				if(rank == 0)
					scan->rawData()->addMeasurement(AMMeasurementInfo(infoName, infoDescription, infoUnits));
				if(rank == 1){
					axisName = lp.at(5);
					axisSize = lp.at(6).toInt();
					axisDescription = lp.at(7);
					axisUnits = lp.at(8);
					AMAxisInfo rank1Info(axisName, axisSize, axisDescription, axisUnits);
					QList<AMAxisInfo> rank1Axes;
					rank1Axes << rank1Info;
					AMMeasurementInfo rank1Measurement(infoName, infoDescription, infoUnits, rank1Axes);
					scan->rawData()->addMeasurement(rank1Measurement);
				}
			}
		}
		else{
			fs >> dataIndex;
			if(dataIndex != -27)
				fs >> dataValue;

			if(dataIndex == -1){
				scan->rawData()->beginInsertRows(1, -1);
				scan->rawData()->setAxisValue(0, insertionIndex, dataValue); // insert eV
			}
			else if(dataIndex == -27){
				scan->rawData()->endInsertRows();
				insertionIndex++;
			}
			else{
				scan->rawData()->setValue(AMnDIndex(insertionIndex), dataIndex, AMnDIndex(), dataValue);
			}
		}
	}

	QString spectraFile = "";
	QFileInfo spectraFileInfo;
	foreach(QString afp, scan->additionalFilePaths())
		if(afp.contains("_spectra.dat"))
			spectraFile = afp;
	if(spectraFile == ""){
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -3, "SGM2013XASFileLoader parse error while loading scan data from file. I couldn't find the the spectra.dat file when I need one."));
		return false;	// bad format; no spectra.dat file in the additional files paths
	}
	spectraFileInfo.setFile(spectraFile);
	if(spectraFileInfo.isRelative())
		spectraFileInfo.setFile(userDataFolder + "/" + spectraFile);
	QFile sf(spectraFileInfo.filePath());
	if(!sf.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "SGM2013XASFileLoader parse error while loading scan data from file. Missing spectra.dat file."));
		return false;
	}

	int measurementIndex;
	int rank1InsertionIndex = -1;
	int measurementSize;
	double axisValue;
	double oneDataValue;
	QVector<double> dataValues;
	int fiveIndex = 0;
	QTextStream sfs(&sf);
	sfs >> measurementIndex;
	while(!sfs.atEnd()){
		if(measurementIndex == -1){
			rank1InsertionIndex++;
			sfs >> axisValue;
		}
		else{
			measurementSize = scan->rawData()->measurementAt(measurementIndex).axes.at(0).size;
			dataValues.clear();
			for(int x = 0; x < measurementSize; x++){
				sfs >> oneDataValue;
				dataValues.append(oneDataValue);
			}
			scan->rawData()->setValue(AMnDIndex(rank1InsertionIndex), measurementIndex, dataValues.constData());
			if(rank1InsertionIndex == 5)
				fiveIndex++;
		}
		sfs >> measurementIndex;
	}

	for(int i=0; i<scan->rawDataSources()->count(); i++) {
		if(scan->rawDataSources()->at(i)->measurementId() >= scan->rawData()->measurementCount()) {
			AMErrorMon::report(AMErrorReport(scan, AMErrorReport::Debug, -97, QString("The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
			scan->deleteRawDataSource(i);
		}
	}


	/// scan->onDataChanged(); \todo Is this still used? What does it mean?
	return true;
}

bool SGM2013XASFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "sgm2013XAS");
}

Q_EXPORT_PLUGIN2(SGM2013XASFileLoaderFactory, SGM2013XASFileLoaderFactory)

