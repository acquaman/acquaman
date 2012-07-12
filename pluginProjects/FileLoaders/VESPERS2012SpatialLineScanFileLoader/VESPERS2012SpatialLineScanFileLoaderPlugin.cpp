#include "VESPERS2012SpatialLineScanFileLoaderPlugin.h"

#include <QtGui>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMScan.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

bool VESPERS2012SpatialLineScanFileLoaderPlugin::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012LineScanXRF1El"
			|| scan->fileFormat() == "vespers2012LineScanXRF1ElXRD"
			|| scan->fileFormat() == "vespers2012LineScanXRF4El"
			|| scan->fileFormat() == "vespers2012LineScanXRF4ElXRD")
		return true;

	return false;
}

bool VESPERS2012SpatialLineScanFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder)
{
	// Check for null scan reference.
	if (!scan)
		return false;

	// Clear the old scan axes to ensure we don't have any extras.
	scan->clearRawDataCompletely();

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	QFile file(sourceFileInfo.filePath());
	if(!file.open(QIODevice::ReadOnly)) {
		AMErrorMon::error(0, -1, "Line Scan File Loader parse error while loading scan data from file.");
		return false;
	}

	QFile spectra;
	QVector<int> data;
	QVector<int> raw1;
	QVector<int> raw2;
	QVector<int> raw3;
	QVector<int> raw4;

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;

	bool usingSingleElement = false;
	bool usingSingleElementAndCCD = false;
	bool usingFourElement = false;
	bool usingFourElementAndCCD = false;

	if (scan->fileFormat() == "vespers2012LineScanXRF1El")
		usingSingleElement = true;
	else if (scan->fileFormat() == "vespers2012LineScanXRF1ElXRD")
		usingSingleElementAndCCD = true;
	else if (scan->fileFormat() == "vespers2012LineScanXRF4El")
		usingFourElement = true;
	else if (scan->fileFormat() == "vespers2012LineScanXRF4ElXRD")
		usingFourElementAndCCD = true;

	if (usingSingleElement || usingSingleElementAndCCD){

		data.resize(2048);

		foreach(QString additionalFilePath, scan->additionalFilePaths())
			if(additionalFilePath.contains("_spectra.dat"))
				spectra.setFileName(additionalFilePath);

		QFileInfo spectraFileInfo(spectra.fileName());
		if (spectraFileInfo.isRelative())
			spectra.setFileName(userDataFolder + "/" + spectra.fileName());
		if(!spectra.open(QIODevice::ReadOnly)) {
			AMErrorMon::error(0, -1, QString("XASFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName()));
			return false;
		}
	}
	else if (usingFourElement || usingSingleElementAndCCD){

		data.resize(2048);
		raw1.resize(2048);
		raw2.resize(2048);
		raw3.resize(2048);
		raw4.resize(2048);

		foreach(QString additionalFilePath, scan->additionalFilePaths())
			if(additionalFilePath.contains("_spectra.dat"))
				spectra.setFileName(additionalFilePath);

		QFileInfo spectraFileInfo(spectra.fileName());
		if (spectraFileInfo.isRelative())
			spectra.setFileName(userDataFolder + "/" + spectra.fileName());
		if(!spectra.open(QIODevice::ReadOnly)) {
			AMErrorMon::error(0, -1, QString("XASFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName()));
			return false;
		}
	}
	else
		spectra.setFileName("");

	QTextStream spectraStream(&spectra);
	QString spectraLine;
	QStringList spectraTokenized;

	in.readLine();
	in.readLine();
	in.readLine();

	// Grab the first PV, it tells us what the axis was.
	line = in.readLine();
	lineTokenized = line.split(" ");
	line = lineTokenized.at(2);

	if (line == "TS1607-2-B21-01:H:user:mm")
		scan->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
	else if (line == "TS1607-2-B21-01:V:user:mm")
		scan->rawData()->addScanAxis(AMAxisInfo("V", 0, "Vertical Position", "mm"));
	else if (line == "SVM1607-2-B21-02:mm")
		scan->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
	else if (line == "SVM1607-2-B21-01:mm")
		scan->rawData()->addScanAxis(AMAxisInfo("Z", 0, "Vertical Position", "mm"));

	in.readLine();
	in.readLine();
	in.readLine();

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();

	if (usingSingleElement){

		// The last raw data source is a spectrum.
		for (int i = 0; i < scan->rawDataSourceCount()-1; i++)
			scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		// Note!  Not general!
		QList<AMAxisInfo> axisInfo;
		AMAxisInfo ai("Energy", 2048, "Energy", "eV");
		ai.increment = 10;
		ai.start = AMNumber(0);
		ai.isUniform = true;
		axisInfo << ai;

		scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(scan->rawDataSourceCount()-1)->name(), scan->rawDataSources()->at(scan->rawDataSourceCount()-1)->description(), "eV", axisInfo));
	}

	else if (usingFourElement){

		// The last 5 raw data sources are spectra.
		for (int i = 0; i < scan->rawDataSourceCount()-5; i++)
			scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		// Note!  Not general!
		QList<AMAxisInfo> axisInfo;
		AMAxisInfo ai("Energy", 2048, "Energy", "eV");
		ai.increment = 10;
		ai.start = AMNumber(0);
		ai.isUniform = true;
		axisInfo << ai;

		for (int i = scan->rawDataSourceCount()-5; i < scan->rawDataSourceCount(); i++)
			scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description(), "eV", axisInfo));
	}

	int position = 0;
	lineTokenized.clear();

	while (!in.atEnd()){

		line = in.readLine();
		lineTokenized << line.split(", ");

		// Add in the data at the right spot.
		AMnDIndex axisValueIndex(position);
		scan->rawData()->beginInsertRowsAsNecessaryForScanPoint(axisValueIndex);

		scan->rawData()->setAxisValue(0, axisValueIndex.i(), lineTokenized.at(1).toDouble());

		// This isn't the most efficient way of putting the spectra data in, but it will do for the time being.
		if (usingSingleElement){

			// Only going to rawDataSourceCount-1 because the last raw data source is the 2D spectra scan and requires its own method of entering the data.
			for (int i = 0; i < scan->rawDataSourceCount()-1; i++)
				scan->rawData()->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());

			spectraTokenized.clear();
			spectraLine = spectraStream.readLine();
			spectraTokenized << spectraLine.split(",");

			for (int j = 0; j < 2048; j++)
				data[j] = spectraTokenized.at(j).toInt();

			scan->rawData()->setValue(axisValueIndex, scan->rawDataSourceCount()-1, data.constData(), data.size());
		}

		else if (usingFourElement){

			// Only going to rawDataSourceCount-5 because the last 5 raw data sources are the 2D spectra scan and requires its own method of entering the data.
			for (int i = 0; i < scan->rawDataSourceCount()-5; i++)
				scan->rawData()->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());

			spectraTokenized.clear();
			spectraLine = spectraStream.readLine();
			spectraTokenized << spectraLine.split(",");

			for (int j = 0; j < 2048; j++){

				data[j] = spectraTokenized.at(j).toInt();
				raw1[j] = spectraTokenized.at(j+2048).toInt();
				raw2[j] = spectraTokenized.at(j+4096).toInt();
				raw3[j] = spectraTokenized.at(j+6144).toInt();
				raw4[j] = spectraTokenized.at(j+8192).toInt();
			}

			scan->rawData()->setValue(axisValueIndex, scan->rawDataSourceCount()-5, data.constData(), data.size());
			scan->rawData()->setValue(axisValueIndex, scan->rawDataSourceCount()-4, raw1.constData(), raw1.size());
			scan->rawData()->setValue(axisValueIndex, scan->rawDataSourceCount()-3, raw2.constData(), raw2.size());
			scan->rawData()->setValue(axisValueIndex, scan->rawDataSourceCount()-2, raw3.constData(), raw3.size());
			scan->rawData()->setValue(axisValueIndex, scan->rawDataSourceCount()-1, raw4.constData(), raw4.size());
		}
		scan->rawData()->endInsertRows();

		// Advance to the next spot.
		position++;
		lineTokenized.clear();
	}

	file.close();

	return true;
}

bool VESPERS2012SpatialLineScanFileLoaderFactory::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012LineScanXRF1El"
			|| scan->fileFormat() == "vespers2012LineScanXRF1ElXRD"
			|| scan->fileFormat() == "vespers2012LineScanXRF4El"
			|| scan->fileFormat() == "vespers2012LineScanXRF4ElXRD")
		return true;

	return false;
}

Q_EXPORT_PLUGIN2(VESPERS2012SpatialLineScanFileLoaderFactory, VESPERS2012SpatialLineScanFileLoaderFactory)

