#include "SGM2013FastFileLoaderPlugin.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

#include "dataman/AMScan.h"
#include "dataman/AMTextStream.h"

bool SGM2013FastFileLoaderPlugin::accepts(AMScan *scan){
	if(scan->fileFormat() == "sgm2013Fast")
		return true;
	return false;
}

bool SGM2013FastFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor){
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
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2013FASTFILELOADERPLUGIN_CANNOT_OPEN_FILE, "SGM2013FastFileLoader parse error while loading scan data from file. Missing file."));
		return false;
	}
	QTextStream fs(&f);

	QMap<int, QList<int> > measurementOrderByRank;

	// used in parsing the data file
	QString line;
	QStringList lp;
	QString versionString;
	int index;
	double dataValue;
	int insertionIndex = 0;
	bool informationSection = false;
	bool finishedHeader = false;
	while(!fs.atEnd()){
		if(!finishedHeader)
			line = fs.readLine();
		if(line == "Start Info")
			informationSection = true;
		else if(line.contains("Version: "))
			versionString = line.remove("Version: ");
		else if(line == "End Info"){
			informationSection = false;
			finishedHeader = true;
			line = "";
		}
		else if(informationSection){
			if(versionString == "SGM Generic Fast 0.1"){
				if(!line.contains("Encoder Start Value:") && !line.contains("SpacingParam:") && !line.contains("C1Param:") && !line.contains("C2Param:") && !line.contains("SParam:") && !line.contains("ThetaParam:")){
					lp = line.split("|!|!|");
					index = lp.at(0).toInt();
					if(index >= 0){
						QString oneString = lp.at(1);
						AMTextStream measurementInfoStreamOut(&oneString);
						AMMeasurementInfo oneMeasurementInfo = AMMeasurementInfo(QString(), QString());
						measurementInfoStreamOut.read(oneMeasurementInfo);

						if(measurementOrderByRank.contains(oneMeasurementInfo.rank())){
							QList<int> thisRankList = measurementOrderByRank.value(oneMeasurementInfo.rank());
							thisRankList.append(index);
							measurementOrderByRank.insert(oneMeasurementInfo.rank(), thisRankList);
						}
						else{
							QList<int> newRankList;
							newRankList.append(index);
							measurementOrderByRank.insert(oneMeasurementInfo.rank(), newRankList);
						}

						scan->rawData()->addMeasurement(oneMeasurementInfo);
					}
				}
			}
		}
		else{
			if(!fs.atEnd()){
				fs >> dataValue;
				if(!fs.atEnd()){
					scan->rawData()->beginInsertRows(1, -1);
					scan->rawData()->setAxisValue(0, insertionIndex, dataValue); // insert eV
					QList<int> rank0Measurements = measurementOrderByRank.value(0);
					for(int x = 0; x < rank0Measurements.count(); x++){
						fs >> dataValue;
						scan->rawData()->setValue(AMnDIndex(insertionIndex), rank0Measurements.at(x), AMnDIndex(), dataValue);
					}
					scan->rawData()->endInsertRows();
					insertionIndex++;
				}
			}
		}
	}

	if(!scan->additionalFilePaths().isEmpty()){
		QString spectraFile = "";
		QFileInfo spectraFileInfo;
		foreach(QString afp, scan->additionalFilePaths())
			if(afp.contains("_spectra.dat"))
				spectraFile = afp;
		if(spectraFile == ""){
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2013FASTFILELOADERPLUGIN_NO_SPECTRA_FILE, "SGM2013FastFileLoader parse error while loading scan data from file. I couldn't find the the spectra.dat file when I need one."));
			return false;	// bad format; no spectra.dat file in the additional files paths
		}
		spectraFileInfo.setFile(spectraFile);
		if(spectraFileInfo.isRelative())
			spectraFileInfo.setFile(userDataFolder + "/" + spectraFile);
		QFile sf(spectraFileInfo.filePath());
		if(!sf.open(QIODevice::ReadOnly)) {
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, SGM2013FASTFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE, "SGM2013FastFileLoader parse error while loading scan data from file. Missing spectra.dat file."));
			return false;
		}

		int rank1InsertionIndex = 0;
		int measurementSize;
		double oneDataValue;
		QVector<double> dataValues;
		QTextStream sfs(&sf);

		QList<int> rank1Measurements = measurementOrderByRank.value(1);
		while(!sfs.atEnd()){
			for(int x = 0; x < rank1Measurements.count(); x++){
				measurementSize = scan->rawData()->measurementAt(rank1Measurements.at(x)).axes.at(0).size;
				dataValues.clear();
				for(int y = 0; y < measurementSize; y++){
					sfs >> oneDataValue;
					dataValues.append(oneDataValue);
				}
				scan->rawData()->setValue(AMnDIndex(rank1InsertionIndex), rank1Measurements.at(x), dataValues.constData());
			}
			rank1InsertionIndex++;
		}
	}

	for(int i=0; i<scan->rawDataSources()->count(); i++) {
		if(scan->rawDataSources()->at(i)->measurementId() >= scan->rawData()->measurementCount()) {
			errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Debug, SGM2013FASTFILELOADERPLUGIN_DATA_COLUMN_MISMATCH, QString("The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
			scan->deleteRawDataSource(i);
		}
	}


	/// scan->onDataChanged(); \todo Is this still used? What does it mean?
	return true;
}

bool SGM2013FastFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "sgm2013Fast");
}

Q_EXPORT_PLUGIN2(SGM2013FastFileLoaderFactory, SGM2013FastFileLoaderFactory)

