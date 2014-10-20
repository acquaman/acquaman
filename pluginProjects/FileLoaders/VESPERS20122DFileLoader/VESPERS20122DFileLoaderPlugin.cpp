#include "VESPERS20122DFileLoaderPlugin.h"

#pragma clang diagnostic ignored "-Wunused-private-field"
#include <QtGui>
#pragma clang diagnostic warning "-Wunused-private-field"

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMScan.h"
#include "dataman/datastore/AMCDFDataStore.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QByteArray>

bool VESPERS20122DFileLoaderPlugin::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012XRF1El"
			|| scan->fileFormat() == "vespers2012XRF1ElXRD"
			|| scan->fileFormat() == "vespers2012XRF4El"
			|| scan->fileFormat() == "vespers2012XRF4ElXRD"
			|| scan->fileFormat() == "vespers2012XRF1Eln4El"
			|| scan->fileFormat() == "vespers2012XRF1Eln4ElXRD")
		return true;

	return false;
}

bool VESPERS20122DFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor)
{
	// Check for null scan reference.
	if (!scan)
		return false;

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	QFile file(sourceFileInfo.filePath());
	if(!file.open(QIODevice::ReadOnly)) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, VESPERS2012DFILELOADERPLUGIN_CANNOT_OPEN_FILE, "2D Map FileLoader parse error while loading scan data from file."));
		return false;
	}

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;

	bool usingSingleElement = false;
	bool usingFourElement = false;

	if (scan->fileFormat() == "vespers2012XRF1El" || scan->fileFormat() == "vespers2012XRF1ElXRD")
		usingSingleElement = true;
	else if (scan->fileFormat() == "vespers2012XRF4El" || scan->fileFormat() == "vespers2012XRF4ElXRD")
		usingFourElement = true;
	else if (scan->fileFormat() == "vespers2012XRF1Eln4El" || scan->fileFormat() == "vespers2012XRF1Eln4ElXRD"){

		usingSingleElement = true;
		usingFourElement = true;
	}

	int count = scan->rawDataSourceCount();

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

	// Determine the number of y lines, since we need to know that before creating the scan axes.
	/////////////////////
	QStringList fileLines;
	while(!in.atEnd())
		fileLines << in.readLine();

	file.close();

	// Some setup variables.
	int x = 0;
	int y = 0;
	int xLength = 0;
	int yLength = 0;

	// Only need to find xLength because yLength can be found by filelines.size()/xLength.
	for (int i = 0, lineCount = fileLines.size(); i < lineCount && xLength == 0; i++){

		QString currentLine = fileLines.at(i);
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
	}

	yLength = fileLines.size()/xLength;

	// If there is a remainder then the last row wasn't completed and we need to add one to the yLength.
	if (fileLines.size()%xLength)
		yLength++;

	QString temp = sourceFileInfo.filePath();
	temp.replace(".dat", ".cdf");
	AMCDFDataStore *cdfData = new AMCDFDataStore(temp, false);

	if (line == "TS1607-2-B21-02:H:user:mm"){

		cdfData->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
		cdfData->addScanAxis(AMAxisInfo("V", yLength, "Vertical Position", "mm"));
	}

	else if (line == "SVM1607-2-B21-02:mm"){

		cdfData->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
		cdfData->addScanAxis(AMAxisInfo("Z", yLength, "Vertical Position", "mm"));
	}

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();

	// Note!  Not general!
	QList<AMAxisInfo> axisInfo;
	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.increment = 10;
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axisInfo << ai;

	if (usingSingleElement && usingFourElement){

		for (int i = 0; i < count-6; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		for (int i = count-6; i < count; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description(), "eV", axisInfo));
	}

	else if (usingSingleElement){

		for (int i = 0; i < count-1; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(count-1)->name(), scan->rawDataSources()->at(count-1)->description(), "eV", axisInfo));
	}

	else if (usingFourElement){

		for (int i = 0; i < count-5; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		for (int i = count-5; i < count; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description(), "eV", axisInfo));
	}

	cdfData->beginInsertRows(xLength, -1);

	x = 0;
	y = 0;

	foreach(QString line, fileLines) {

		lineTokenized = line.split(", ");

		// Add in the data at the right spot.
		AMnDIndex axisValueIndex(x, y);

		if (axisValueIndex.j() == 0)
			cdfData->setAxisValue(0, axisValueIndex.i(), lineTokenized.at(1).toDouble());

		if (axisValueIndex.i() == 0)
			cdfData->setAxisValue(1, axisValueIndex.j(), lineTokenized.at(2).toDouble());

		if (usingSingleElement && usingFourElement){

			for (int i = 0; i < count-6; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+3).toDouble());
		}

		else if (usingSingleElement){

			for (int i = 0; i < count-1; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+3).toDouble());
		}

		else if (usingFourElement){

			for (int i = 0; i < count-5; i++)
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

			if (usingSingleElement && usingFourElement){

				for (int i = 0; i < count-6; i++)
					cdfData->setValue(axisValueIndex, i, AMnDIndex(), -1);
			}

			else if (usingSingleElement){

				for (int i = 0; i < count-1; i++)
					cdfData->setValue(axisValueIndex, i, AMnDIndex(), -1);
			}

			else if (usingFourElement){

				for (int i = 0; i < count-5; i++)
					cdfData->setValue(axisValueIndex, i, AMnDIndex(), -1);
			}
		}
	}

	// Getting the spectra file setup.
	if (usingSingleElement || usingFourElement){

		QFile spectra;

		foreach(QString additionalFilePath, scan->additionalFilePaths())
			if(additionalFilePath.contains("_spectra.dat"))
				spectra.setFileName(additionalFilePath);

		QFileInfo spectraFileInfo(spectra.fileName());
		if (spectraFileInfo.isRelative())
			spectra.setFileName(userDataFolder + "/" + spectra.fileName());
		if(!spectra.open(QIODevice::ReadOnly)) {
			errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, VESPERS2012DFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE, QString("2DFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName())));
			return false;
		}

		if (usingSingleElement && usingFourElement){

			QVector<int> data(2048*6);
			QVector<int> fill(2048, -1);

			for (int y = 0, ySize = cdfData->scanSize(1); y < ySize; y++){

				for (int x = 0, xSize = cdfData->scanSize(0); x < xSize; x++){

					QByteArray row = spectra.readLine();

					if (!row.isEmpty()){

						bool insideWord = false;
						QString word;
						word.reserve(12);
						int dataIndex = 0;

						for (int byte = 0, rowLength = row.length(); byte < rowLength; byte++){

							char c = row.at(byte);

							if (c == ','){

								// The end of a word, so convert it to an int.
								data[dataIndex++] = word.toInt();
								word.clear();
								insideWord = false;
							}

							else {

								// Still inside a word.
								word.append(c);
								insideWord = true;
							}
						}

						// Possible last word.
						if (insideWord)
							data[dataIndex] = word.toInt();

						const int *tempData = data.constData();
						AMnDIndex axisValueIndex(x, y);
						cdfData->setValue(axisValueIndex, count-6, tempData);
						cdfData->setValue(axisValueIndex, count-5, tempData+2048);
						cdfData->setValue(axisValueIndex, count-4, tempData+4096);
						cdfData->setValue(axisValueIndex, count-3, tempData+6144);
						cdfData->setValue(axisValueIndex, count-2, tempData+8192);
						cdfData->setValue(axisValueIndex, count-1, tempData+10240);
					}

					else {

						AMnDIndex axisValueIndex(x, y);
						cdfData->setValue(axisValueIndex, count-6, fill.constData());
						cdfData->setValue(axisValueIndex, count-5, fill.constData());
						cdfData->setValue(axisValueIndex, count-4, fill.constData());
						cdfData->setValue(axisValueIndex, count-3, fill.constData());
						cdfData->setValue(axisValueIndex, count-2, fill.constData());
						cdfData->setValue(axisValueIndex, count-1, fill.constData());
					}
				}
			}

			spectra.close();
		}

		else if (usingSingleElement){

			QVector<int> data(2048);
			QVector<int> fill(2048, -1);

			for (int y = 0, ySize = cdfData->scanSize(1); y < ySize; y++){

				for (int x = 0, xSize = cdfData->scanSize(0); x < xSize; x++){

					QByteArray row = spectra.readLine();

					// If the row is empty then that will mean that there is no data to be read.
					if (!row.isEmpty()){

						bool insideWord = false;
						QString word;
						word.reserve(12);
						int dataIndex = 0;

						for (int byte = 0, rowLength = row.length(); byte < rowLength; byte++){

							char c = row.at(byte);

							if (c == ','){

								// The end of a word, so convert it to an int.
								data[dataIndex++] = word.toInt();
								word.clear();
								insideWord = false;
							}

							else {

								// Still inside a word.
								word.append(c);
								insideWord = true;
							}
						}

						// Possible last word.
						if (insideWord)
							data[dataIndex] = word.toInt();

						cdfData->setValue(AMnDIndex(x, y), count-1, data.constData());
					}

					else{

						cdfData->setValue(AMnDIndex(x, y), count-1, fill.constData());
					}
				}
			}

			spectra.close();
		}

		else if (usingFourElement){

			QVector<int> data(2048*5);
			QVector<int> fill(2048, -1);

			for (int y = 0, ySize = cdfData->scanSize(1); y < ySize; y++){

				for (int x = 0, xSize = cdfData->scanSize(0); x < xSize; x++){

					QByteArray row = spectra.readLine();

					if (!row.isEmpty()){

						bool insideWord = false;
						QString word;
						word.reserve(12);
						int dataIndex = 0;

						for (int byte = 0, rowLength = row.length(); byte < rowLength; byte++){

							char c = row.at(byte);

							if (c == ','){

								// The end of a word, so convert it to an int.
								data[dataIndex++] = word.toInt();
								word.clear();
								insideWord = false;
							}

							else {

								// Still inside a word.
								word.append(c);
								insideWord = true;
							}
						}

						// Possible last word.
						if (insideWord)
							data[dataIndex] = word.toInt();

						const int *tempData = data.constData();
						AMnDIndex axisValueIndex(x, y);
						cdfData->setValue(axisValueIndex, count-5, tempData);
						cdfData->setValue(axisValueIndex, count-4, tempData+2048);
						cdfData->setValue(axisValueIndex, count-3, tempData+4096);
						cdfData->setValue(axisValueIndex, count-2, tempData+6144);
						cdfData->setValue(axisValueIndex, count-1, tempData+8192);
					}

					else {

						AMnDIndex axisValueIndex(x, y);
						cdfData->setValue(axisValueIndex, count-5, fill.constData());
						cdfData->setValue(axisValueIndex, count-4, fill.constData());
						cdfData->setValue(axisValueIndex, count-3, fill.constData());
						cdfData->setValue(axisValueIndex, count-2, fill.constData());
						cdfData->setValue(axisValueIndex, count-1, fill.constData());
					}
				}
			}

			spectra.close();
		}
	}

	cdfData->endInsertRows();

	if (!scan->replaceRawDataStore(cdfData))
		return false;

	scan->setFileFormat("amCDFv1");
	scan->setFilePath(temp);

	return scan->storeToDb(scan->database());
}

bool VESPERS20122DFileLoaderFactory::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012XRF1El"
			|| scan->fileFormat() == "vespers2012XRF1ElXRD"
			|| scan->fileFormat() == "vespers2012XRF4El"
			|| scan->fileFormat() == "vespers2012XRF4ElXRD"
			|| scan->fileFormat() == "vespers2012XRF1Eln4El"
			|| scan->fileFormat() == "vespers2012XRF1Eln4ElXRD")
		return true;

	return false;
}

Q_EXPORT_PLUGIN2(VESPERS20122DFileLoaderFactory, VESPERS20122DFileLoaderFactory)

