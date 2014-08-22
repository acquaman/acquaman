#include "AMRegion2013FileLoaderPlugin.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "analysis/AM2DSummingAB.h"

#include "dataman/AMScan.h"
#include "dataman/AMTextStream.h"

bool AMRegion2013FileLoaderPlugin::accepts(AMScan *scan)
{
	 return scan->fileFormat() == "amRegionAscii2013";
}

bool AMRegion2013FileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor)
{
	if(!scan)
		return false;

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	// open the file:
	QFile f(sourceFileInfo.filePath());
	if(!f.open(QIODevice::ReadOnly)) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, AMREGION2013FILELOADERPLUGIN_CANNOT_OPEN_FILE, "SGM2013XASFileLoader parse error while loading scan data from file. Missing file."));
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
			if(versionString == "Acquaman Generic Linear Step 0.1"){
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

				else {
					QString oneString = lp.at(1);
					AMTextStream axisInfoStreamOut(&oneString);
					AMAxisInfo axisInfo = AMAxisInfo("tempName", 0);

					// This is a catch all for old scans before we properly saved the scan axes.
					if (lp.at(1) == "eV" && !lp.at(1).contains("|@|@|"))
						scan->rawData()->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

					else{
						axisInfoStreamOut.read(axisInfo);
						scan->rawData()->addScanAxis(axisInfo);
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
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, AMREGION2013FILELOADERPLUGIN_NO_SPECTRA_FILE, "SGM2013XASFileLoader parse error while loading scan data from file. I couldn't find the the spectra.dat file when I need one."));
			return false;	// bad format. No spectra.dat file in the additional files paths
		}
		spectraFileInfo.setFile(spectraFile);
		if(spectraFileInfo.isRelative())
			spectraFileInfo.setFile(userDataFolder + "/" + spectraFile);
		QFile sf(spectraFileInfo.filePath());
		if(!sf.open(QIODevice::ReadOnly)) {
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, AMREGION2013FILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE, "SGM2013XASFileLoader parse error while loading scan data from file. Missing spectra.dat file."));
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
			errorMonitor->exteriorReport(AMErrorReport(scan, AMErrorReport::Debug, AMREGION2013FILELOADERPLUGIN_DATA_COLUMN_MISMATCH, QString("The data in the file (%1 columns) didn't match the raw data columns we were expecting (column %2). Removing the raw data column '%3')").arg(scan->rawData()->measurementCount()).arg(scan->rawDataSources()->at(i)->measurementId()).arg(scan->rawDataSources()->at(i)->name())));
			scan->deleteRawDataSource(i);
		}
	}

	return true;
}

bool AMRegion2013FileLoaderFactory::accepts(AMScan *scan)
{
	return scan->fileFormat() == "amRegionAscii2013";
}

Q_EXPORT_PLUGIN2(AMRegion2013FileLoaderFactory, AMRegion2013FileLoaderFactory)

