#include "VESPERS20122DFileLoaderPlugin.h"

#include <QtGui>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMScan.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

bool VESPERS20122DFileLoaderPlugin::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012XRF1El"
			|| scan->fileFormat() == "vespers2012XRF1ElXRD"
			|| scan->fileFormat() == "vespers2012XRF4El"
			|| scan->fileFormat() == "vespers2012XRF4ElXRD")
		return true;

	return false;
}

bool VESPERS20122DFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder)
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
		AMErrorMon::error(0, -1, "2D Map FileLoader parse error while loading scan data from file.");
		return false;
	}

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;

	bool usingSingleElement = false;
	bool usingSingleElementAndCCD = false;
	bool usingFourElement = false;
	bool usingFourElementAndCCD = false;

	if (scan->fileFormat() == "vespers2012XRF1El")
		usingSingleElement = true;
	else if (scan->fileFormat() == "vespers2012XRF1ElXRD")
		usingSingleElementAndCCD = true;
	else if (scan->fileFormat() == "vespers2012XRF4El")
		usingFourElement = true;
	else if (scan->fileFormat() == "vespers2012XRF4ElXRD")
		usingFourElementAndCCD = true;


	in.readLine();
	in.readLine();
	in.readLine();

	// Grab the first PV, it tells us what the axis was.
	line = in.readLine();
	lineTokenized = line.split(" ");
	line = lineTokenized.at(2);

	if (line == "TS1607-2-B21-01:H:user:mm"){

		scan->rawData()->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		scan->rawData()->addScanAxis(AMAxisInfo("V", 0, "Vertical Position", "mm"));
	}

	else if (line == "SVM1607-2-B21-02:mm"){

		scan->rawData()->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		scan->rawData()->addScanAxis(AMAxisInfo("Z", 0, "Vertical Position", "mm"));
	}

	in.readLine();
	in.readLine();
	in.readLine();
	lineTokenized.clear();

	// Some setup variables.
	int x = 0;
	int y = 0;
	int xLength = 0;

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();

	// Include all for now.
	for (int i = 0; i < scan->rawDataSourceCount(); i++)
		scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

	while (!in.atEnd()){

		line = in.readLine();
		lineTokenized << line.split(", ");

		// Used for determining how long the x axis is.
		if (xLength == 0 && lineTokenized.at(1).toDouble() == double(scan->rawData()->axisValue(0, 0))){

			xLength = x;
			x = 0;
			y++;
		}

		// Add in the data at the right spot.
		AMnDIndex axisValueIndex(x, y);
		scan->rawData()->beginInsertRowsAsNecessaryForScanPoint(axisValueIndex);

		scan->rawData()->setAxisValue(0, axisValueIndex.i(), lineTokenized.at(1).toDouble());
		scan->rawData()->setAxisValue(1, axisValueIndex.j(), lineTokenized.at(2).toDouble());

		for (int i = 0; i < scan->rawDataSourceCount(); i++)
			scan->rawData()->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+3).toDouble());

		scan->rawData()->endInsertRows();

		// Advance to the next spot.
		x++;

		if (xLength != 0 && x == xLength){

			x = 0;
			y++;
		}

		lineTokenized.clear();
	}

	// Pad the rest of the line with zeroes for proper visualization.
	if (x != 0 && xLength != 0){

		for ( ; x < xLength; x++){

			// Add in the data at the right spot.
			AMnDIndex axisValueIndex(x, y);
			scan->rawData()->beginInsertRowsAsNecessaryForScanPoint(axisValueIndex);

			scan->rawData()->setAxisValue(0, axisValueIndex.i(), scan->rawData()->axisValue(0, axisValueIndex.i()));
			scan->rawData()->setAxisValue(1, axisValueIndex.j(), scan->rawData()->axisValue(1, axisValueIndex.j()-1));

			for (int i = 0; i < scan->rawDataSourceCount(); i++)
				scan->rawData()->setValue(axisValueIndex, i, AMnDIndex(), 0);

			scan->rawData()->endInsertRows();
		}
	}

	file.close();

	return true;
}

bool VESPERS20122DFileLoaderFactory::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012XRF1El"
			|| scan->fileFormat() == "vespers2012XRF1ElXRD"
			|| scan->fileFormat() == "vespers2012XRF4El"
			|| scan->fileFormat() == "vespers2012XRF4ElXRD")
		return true;

	return false;
}

Q_EXPORT_PLUGIN2(VESPERS20122DFileLoaderFactory, VESPERS20122DFileLoaderFactory)

