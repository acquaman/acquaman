#include "VESPERS2012SpatialLineScanFileLoaderPlugin.h"

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

bool VESPERS2012SpatialLineScanFileLoaderPlugin::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012LineScanXRF1El"
			|| scan->fileFormat() == "vespers2012LineScanXRF1ElXRD"
			|| scan->fileFormat() == "vespers2012LineScanXRF4El"
			|| scan->fileFormat() == "vespers2012LineScanXRF4ElXRD"
			|| scan->fileFormat() == "vespers2012LineScan1Eln4El"
			|| scan->fileFormat() == "vespers2012LineScan1Eln4ElXRD"
			|| scan->fileFormat() == "vespers2012Energy")
		return true;

	return false;
}

bool VESPERS2012SpatialLineScanFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor)
{
	// Check for null scan reference.
	if (!scan)
		return false;

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	QFile file(sourceFileInfo.filePath());
	if(!file.open(QIODevice::ReadOnly)) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, VESPERS201SPATIALLINESCANFILELOADERPLUGIN_CANNOT_OPEN_FILE, "Line Scan File Loader parse error while loading scan data from file."));
		return false;
	}

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;

	bool usingSingleElement = false;
	bool usingFourElement = false;

	if (scan->fileFormat() == "vespers2012LineScanXRF1El" || scan->fileFormat() == "vespers2012LineScanXRF1ElXRD")
		usingSingleElement = true;
	else if (scan->fileFormat() == "vespers2012LineScanXRF4El" || scan->fileFormat() == "vespers2012LineScanXRF4ElXRD")
		usingFourElement = true;
	else if (scan->fileFormat() == "vespers2012LineScan1Eln4El" || scan->fileFormat() == "vespers2012LineScan1Eln4ElXRD"){

		usingSingleElement = true;
		usingFourElement = true;
	}

	in.readLine();
	in.readLine();
	in.readLine();

	// Grab the first PV, it tells us what the axis was.
	line = in.readLine();
	lineTokenized = line.split(" ");
	line = lineTokenized.at(2);

	QString temp = sourceFileInfo.filePath();
	temp.replace(".dat", ".cdf");
	AMCDFDataStore *cdfData = new AMCDFDataStore(temp, false);

	if (line == "TS1607-2-B21-01:H:user:mm")
		cdfData->addScanAxis(AMAxisInfo("H", 0, "Horizontal Position", "mm"));
	else if (line == "TS1607-2-B21-01:V:user:mm")
		cdfData->addScanAxis(AMAxisInfo("V", 0, "Vertical Position", "mm"));
	else if (line == "SVM1607-2-B21-02:mm")
		cdfData->addScanAxis(AMAxisInfo("X", 0, "Horizontal Position", "mm"));
	else if (line == "SVM1607-2-B21-01:mm")
		cdfData->addScanAxis(AMAxisInfo("Z", 0, "Vertical Position", "mm"));
	else if (line == "07B2_Mono_SineB_Egec:eV")
		cdfData->addScanAxis(AMAxisInfo("eV", 0, "Incident Energy", "eV"));

	in.readLine();
	in.readLine();
	in.readLine();

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();
	int axisValueIndex = 0;
	int count = scan->rawDataSourceCount();

	// Note!  Not general!
	QList<AMAxisInfo> axisInfo;
	AMAxisInfo ai("Energy", 2048, "Energy", "eV");
	ai.increment = 10;
	ai.start = AMNumber(0);
	ai.isUniform = true;
	axisInfo << ai;

	if (usingSingleElement && usingFourElement){

		// The last 6 raw data sources are spectra.
		for (int i = 0; i < count-6; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		for (int i = count-6; i < count; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description(), "eV", axisInfo));
	}

	else if (usingSingleElement){

		// The last raw data source is a spectrum.
		for (int i = 0; i < count-1; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(scan->rawDataSourceCount()-1)->name(), scan->rawDataSources()->at(scan->rawDataSourceCount()-1)->description(), "eV", axisInfo));
	}

	else if (usingFourElement){

		// The last 5 raw data sources are spectra.
		for (int i = 0; i < count-5; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		for (int i = count-5; i < count; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description(), "eV", axisInfo));
	}

	else{

		for (int i = 0; i < count; i++)
			cdfData->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

	}

	lineTokenized.clear();

	QStringList fileLines;
	while(!in.atEnd())
		fileLines << in.readLine();

	file.close();

	// Add in the data at the right spot.
	cdfData->beginInsertRows(fileLines.size(), -1);

	foreach(QString currentLine, fileLines){

		lineTokenized = currentLine.split(", ");

		cdfData->setAxisValue(0, axisValueIndex, lineTokenized.at(1).toDouble());

		// This isn't the most efficient way of putting the spectra data in, but it will do for the time being.
		if (usingSingleElement && usingFourElement){

			// Only going to rawDataSourceCount-6 because the last 6 raw data sources are the 2D spectra scan and requires its own method of entering the data.
			for (int i = 0; i < count-6; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());
		}

		else if (usingSingleElement){

			// Only going to rawDataSourceCount-1 because the last raw data source is the 2D spectra scan and requires its own method of entering the data.
			for (int i = 0; i < count-1; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());
		}

		else if (usingFourElement){

			// Only going to rawDataSourceCount-5 because the last 5 raw data sources are the 2D spectra scan and requires its own method of entering the data.
			for (int i = 0; i < count-5; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());
		}

		else{

			for (int i = 0; i < count; i++)
				cdfData->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());
		}

		// Advance to the next spot.
		axisValueIndex++;
	}

	if (usingSingleElement || usingFourElement){

		QFile spectra;

		foreach(QString additionalFilePath, scan->additionalFilePaths())
			if(additionalFilePath.contains("_spectra.dat"))
				spectra.setFileName(additionalFilePath);

		if(spectra.fileName() == ""){

			// Needed until the non-trivial database upgrade happens so I can set all the additional filepaths.
			QString temp(sourceFileInfo.filePath());
			temp.chop(4);
			spectra.setFileName(temp+"_spectra.dat");
		}

		else {

			QFileInfo spectraFileInfo(spectra.fileName());
			if (spectraFileInfo.isRelative())
				spectra.setFileName(userDataFolder + "/" + spectra.fileName());
			if(!spectra.open(QIODevice::ReadOnly)) {
				errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, VESPERS201SPATIALLINESCANFILELOADERPLUGIN_CANNOT_OPEN_SPECTRA_FILE, QString("XASFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName())));
				return false;
			}
		}

		if (usingSingleElement && usingFourElement){

			QVector<int> data(2048*6);

			for (int x = 0, xSize = cdfData->scanSize(0); x < xSize; x++){

				QByteArray row = spectra.readLine();
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
				cdfData->setValue(x, count-6, tempData);
				cdfData->setValue(x, count-5, tempData+2048);
				cdfData->setValue(x, count-4, tempData+4096);
				cdfData->setValue(x, count-3, tempData+6144);
				cdfData->setValue(x, count-2, tempData+8192);
				cdfData->setValue(x, count-1, tempData+10240);
			}
		}

		else if (usingSingleElement){

			QVector<int> data(2048);

			for (int x = 0, xSize = cdfData->scanSize(0); x < xSize; x++){

				QByteArray row = spectra.readLine();
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

				cdfData->setValue(x, count-1, data.constData());
			}
		}

		else if (usingFourElement){

			QVector<int> data(2048*5);

			for (int x = 0, xSize = cdfData->scanSize(0); x < xSize; x++){

				QByteArray row = spectra.readLine();
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
				cdfData->setValue(x, count-5, tempData);
				cdfData->setValue(x, count-4, tempData+2048);
				cdfData->setValue(x, count-3, tempData+4096);
				cdfData->setValue(x, count-2, tempData+6144);
				cdfData->setValue(x, count-1, tempData+8192);
			}
		}

		spectra.close();
	}

	cdfData->endInsertRows();

	if (!scan->replaceRawDataStore(cdfData))
		return false;

	scan->setFileFormat("amCDFv1");
	scan->setFilePath(temp);

	return scan->storeToDb(scan->database());
}

bool VESPERS2012SpatialLineScanFileLoaderFactory::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespers2012LineScanXRF1El"
			|| scan->fileFormat() == "vespers2012LineScanXRF1ElXRD"
			|| scan->fileFormat() == "vespers2012LineScanXRF4El"
			|| scan->fileFormat() == "vespers2012LineScanXRF4ElXRD"
			|| scan->fileFormat() == "vespers2012LineScan1Eln4El"
			|| scan->fileFormat() == "vespers2012LineScan1Eln4ElXRD"
			|| scan->fileFormat() == "vespers2012Energy")
		return true;

	return false;
}

Q_EXPORT_PLUGIN2(VESPERS2012SpatialLineScanFileLoaderFactory, VESPERS2012SpatialLineScanFileLoaderFactory)

