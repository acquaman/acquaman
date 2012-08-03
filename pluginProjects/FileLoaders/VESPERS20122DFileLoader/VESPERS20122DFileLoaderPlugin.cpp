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

	// Necessary until I do a database upgrade.
	bool containsSpectraDataSource = false;
	int count = scan->rawDataSourceCount();

	for (int i = 0; i < count; i++)
		if (scan->rawDataSources()->at(i)->name().contains(QRegExp("spectra|corrSum|raw1|raw2|raw3|raw4")))
			containsSpectraDataSource = true;

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

	// Determine the number of y lines, since we need to know that before creating the scan axes.
	/////////////////////
	QStringList fileLines;
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

	// Note!  Not general!
	QList<AMAxisInfo> axisInfo;
	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.increment = 10;
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axisInfo << ai;

	if ((usingSingleElement || usingSingleElementAndCCD) && containsSpectraDataSource){

		for (int i = 0; i < count-1; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(count-1)->name(), scan->rawDataSources()->at(count-1)->description(), "eV", axisInfo));
	}

	else if ((usingFourElement || usingFourElementAndCCD) && containsSpectraDataSource){

		for (int i = 0; i < count-5; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		for (int i = count-5; i < count; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description(), "eV", axisInfo));
	}
	// Necessary until I do a database upgrade.
	else{

		for (int i = 0; i < count; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));
	}

	cdfData->beginInsertRows(xLength, -1);

	x = 0;
	y = 0;

	foreach(QString line, fileLines) {

		lineTokenized = line.split(", ");

		// Add in the data at the right spot.
		AMnDIndex axisValueIndex(x, y);

		cdfData->setAxisValue(0, axisValueIndex.i(), lineTokenized.at(1).toDouble());
		cdfData->setAxisValue(1, axisValueIndex.j(), lineTokenized.at(2).toDouble());

		if ((usingSingleElement || usingSingleElementAndCCD) && containsSpectraDataSource){

			for (int i = 0; i < count-1; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+3).toDouble());
		}

		else if ((usingFourElement || usingFourElementAndCCD) && containsSpectraDataSource){

			for (int i = 0; i < count-5; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+3).toDouble());
		}
		// Necessary until I have a database upgrade.
		else{

			for (int i = 0; i < count; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+3).toDouble());
		}

		// Advance to the next spot.
		x++;

		if (x == xLength){

			x = 0;
			y++;
		}
	}

	// Pad the rest of the line with -1 for proper visualization.
	if (x != 0 && xLength != 0){

		for ( ; x < xLength; x++){

			// Add in the data at the right spot.
			AMnDIndex axisValueIndex(x, y);

			cdfData->setAxisValue(0, axisValueIndex.i(), cdfData->axisValue(0, axisValueIndex.i()));
			cdfData->setAxisValue(1, axisValueIndex.j(), cdfData->axisValue(1, axisValueIndex.j()-1));

			// Necessary until I have done the database upgrade.
			if ((usingSingleElement || usingSingleElementAndCCD) && containsSpectraDataSource){

				for (int i = 0; i < count-1; i++)
					cdfData->setValue(axisValueIndex, i, AMnDIndex(), -1);
			}

			else if ((usingFourElement || usingFourElementAndCCD) && containsSpectraDataSource){

				for (int i = 0; i < count-5; i++)
					cdfData->setValue(axisValueIndex, i, AMnDIndex(), -1);
			}
		}
	}

	file.close();

	// Until I do a database upgrade, I don't want to do anything with the spectra file unless the data sources already exist.
	if (containsSpectraDataSource){

		// Getting the spectra file setup.
		if (usingSingleElement || usingSingleElementAndCCD){

			QFile spectra;
			QVector<int> data(2048);

			foreach(QString additionalFilePath, scan->additionalFilePaths())
				if(additionalFilePath.contains("_spectra.dat"))
					spectra.setFileName(additionalFilePath);

			QFileInfo spectraFileInfo(spectra.fileName());
			if (spectraFileInfo.isRelative())
				spectra.setFileName(userDataFolder + "/" + spectra.fileName());
			if(!spectra.open(QIODevice::ReadOnly)) {
				AMErrorMon::error(0, -1, QString("2DFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName()));
				return false;
			}

			QTextStream spectraStream(&spectra);
			QString spectraLine;
			QStringList spectraTokenized;
			x = 0;
			y = 0;

			while(!spectraStream.atEnd()){

				spectraLine = spectraStream.readLine();
				spectraTokenized = spectraLine.split(",");

				for (int j = 0; j < 2048; j++)
					data[j] = spectraTokenized.at(j).toInt();

				cdfData->setValue(AMnDIndex(x, y), count-1, data.constData());

				// Advance to the next spot.
				x++;

				if (x == xLength){

					x = 0;
					y++;
				}
			}

			// Pad the rest of the line with -1 for proper visualization.
			if (x != 0 && xLength != 0){

				data.fill(0);

				for ( ; x < xLength; x++)
					cdfData->setValue(AMnDIndex(x, y), count-1, data.constData());
			}

			spectra.close();
		}
		else if (usingFourElement || usingFourElementAndCCD){

			QFile spectra;
			QVector<int> data(2048);
			QVector<int> raw1(2048);
			QVector<int> raw2(2048);
			QVector<int> raw3(2048);
			QVector<int> raw4(2048);

			foreach(QString additionalFilePath, scan->additionalFilePaths())
				if(additionalFilePath.contains("_spectra.dat"))
					spectra.setFileName(additionalFilePath);

			QFileInfo spectraFileInfo(spectra.fileName());
			if (spectraFileInfo.isRelative())
				spectra.setFileName(userDataFolder + "/" + spectra.fileName());
			if(!spectra.open(QIODevice::ReadOnly)) {
				AMErrorMon::error(0, -1, QString("2DFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName()));
				return false;
			}

			QTextStream spectraStream(&spectra);
			QString spectraLine;
			QStringList spectraTokenized;
			x = 0;
			y = 0;

			while(!spectraStream.atEnd()){

				spectraLine = spectraStream.readLine();
				spectraTokenized = spectraLine.split(",");

				for (int j = 0; j < 2048; j++){

					data[j] = spectraTokenized.at(j).toInt();
					raw1[j] = spectraTokenized.at(j+2048).toInt();
					raw2[j] = spectraTokenized.at(j+4096).toInt();
					raw3[j] = spectraTokenized.at(j+6144).toInt();
					raw4[j] = spectraTokenized.at(j+8192).toInt();
				}

				AMnDIndex axisValueIndex(x, y);
				cdfData->setValue(axisValueIndex, count-5, data.constData());
				cdfData->setValue(axisValueIndex, count-4, raw1.constData());
				cdfData->setValue(axisValueIndex, count-3, raw2.constData());
				cdfData->setValue(axisValueIndex, count-2, raw3.constData());
				cdfData->setValue(axisValueIndex, count-1, raw4.constData());

				// Advance to the next spot.
				x++;

				if (x == xLength){

					x = 0;
					y++;
				}
			}

			// Pad the rest of the line with -1 for proper visualization.
			if (x != 0 && xLength != 0){

				data.fill(0);
				raw1.fill(0);
				raw2.fill(0);
				raw3.fill(0);
				raw4.fill(0);

				for ( ; x < xLength; x++){

					AMnDIndex axisValueIndex(x, y);
					cdfData->setValue(axisValueIndex, count-5, data.constData());
					cdfData->setValue(axisValueIndex, count-4, raw1.constData());
					cdfData->setValue(axisValueIndex, count-3, raw2.constData());
					cdfData->setValue(axisValueIndex, count-2, raw3.constData());
					cdfData->setValue(axisValueIndex, count-1, raw4.constData());
				}
			}

			spectra.close();
		}
	}

	cdfData->endInsertRows();

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

