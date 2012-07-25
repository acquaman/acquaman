#include "VESPERS20122DFileLoaderPlugin.h"

#include <QtGui>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMScan.h"
#include "dataman/datastore/AMCDFDataStore.h"

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

	AMCDFDataStore *cdfData = new AMCDFDataStore;

	// Moved down below, once we know how long the axes are: (Mark, May 13 2012)
//	cdfData->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
//	cdfData->addScanAxis(AMAxisInfo("V", 0, "Vertical Position", "mm"));


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

	in.readLine();
	in.readLine();
	in.readLine();
	lineTokenized.clear();

	// Some setup variables.
	int x = 0;
	int y = 0;
	int xLength = 0;
	int yLength = 0;

	// Include all for now.
	for (int i = 0; i < scan->rawDataSourceCount(); i++)
		cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

	// added by Mark (May 13, 2012) to determine the number of y lines, since we need to know that before creating the scan axes.
	/////////////////////
	QStringList fileLines;
//	fileLines << line;	// we have the first valid line already.
	while(!in.atEnd())
		fileLines << in.readLine();

	foreach(QString currentLine, fileLines) {
		QStringList lineTokenized = currentLine.split(", ");
		double startingXValue;
		// Used for determining how long the x axis is.
		if(x == 0 && xLength == 0)
			startingXValue = lineTokenized.at(1).toDouble();

		// once we wrap around to that x value again, it's the beginning of the next y line.
		if(x != 0 && xLength == 0 && lineTokenized.at(1).toDouble() == startingXValue) {
			xLength = x;
			x = 0;
			y++;
		}

		x++;

		if (xLength != 0 && x == xLength){

			x = 0;
			y++;
		}
	}

	yLength = y; // y is the largest y-index.

	if (line == "TS1607-2-B21-01:H:user:mm"){

		cdfData->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		cdfData->addScanAxis(AMAxisInfo("V", yLength, "Vertical Position", "mm"));
	}

	else if (line == "SVM1607-2-B21-02:mm"){

		cdfData->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		cdfData->addScanAxis(AMAxisInfo("Z", yLength, "Vertical Position", "mm"));
	}

	cdfData->beginInsertRows(xLength, -1);
	///////////////////

	x = 0;
	y = 0;

	foreach(QString line, fileLines) {

		// MB: no longer necessary:
//		// The first time we enter this loop we'll already have the first line of data.
//		if (!(x == 0 && y == 0))
//			line = in.readLine();

//		lineTokenized << line.split(", ");
		lineTokenized = line.split(", "); // MB: is more efficient

		// Used for determining how long the x axis is.
//		if (xLength == 0 && lineTokenized.at(1).toDouble() == double(cdfData->axisValue(0, 0))){

//			xLength = x;
//			x = 0;
//			y++;
//		}

		// Add in the data at the right spot.
		AMnDIndex axisValueIndex(x, y);
		// MB: not needed: all rows added already: cdfData->beginInsertRowsAsNecessaryForScanPoint(axisValueIndex);

		cdfData->setAxisValue(0, axisValueIndex.i(), lineTokenized.at(1).toDouble());
		cdfData->setAxisValue(1, axisValueIndex.j(), lineTokenized.at(2).toDouble());

		for (int i = 0; i < scan->rawDataSourceCount(); i++)
			cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+3).toDouble());

		// MB: moving down to where we're all done filling everything: cdfData->endInsertRows();

		// Advance to the next spot.
		x++;

		if (x == xLength){

			x = 0;
			y++;
		}

//		lineTokenized.clear();
	}

	// Pad the rest of the line with -1 for proper visualization.
	if (x != 0 && xLength != 0){

		for ( ; x < xLength; x++){

			// Add in the data at the right spot.
			AMnDIndex axisValueIndex(x, y);
//MB: 		cdfData->beginInsertRowsAsNecessaryForScanPoint(axisValueIndex);

			cdfData->setAxisValue(0, axisValueIndex.i(), cdfData->axisValue(0, axisValueIndex.i()));
			cdfData->setAxisValue(1, axisValueIndex.j(), cdfData->axisValue(1, axisValueIndex.j()-1));

			for (int i = 0; i < scan->rawDataSourceCount(); i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), -1);

//MB:		cdfData->endInsertRows();
		}
	}

	cdfData->endInsertRows();

	file.close();

	return scan->replaceRawDataStore(cdfData);
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

