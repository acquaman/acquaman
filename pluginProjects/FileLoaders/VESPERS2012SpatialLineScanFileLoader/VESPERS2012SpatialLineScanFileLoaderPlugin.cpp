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

	// Include all for now.
	for (int i = 0; i < scan->rawDataSourceCount(); i++)
		scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

	int position = 0;
	lineTokenized.clear();

	while (!in.atEnd()){

		line = in.readLine();
		lineTokenized << line.split(", ");

		// Add in the data at the right spot.
		AMnDIndex axisValueIndex(position);
		scan->rawData()->beginInsertRowsAsNecessaryForScanPoint(axisValueIndex);

		scan->rawData()->setAxisValue(0, axisValueIndex.i(), lineTokenized.at(1).toDouble());

		for (int i = 0; i < scan->rawDataSourceCount(); i++)
			scan->rawData()->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());

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

