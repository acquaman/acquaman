#include "VESPERS2011XASFileLoaderPlugin.h"

#include <QtGui>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMScan.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

bool VESPERS2011XASFileLoaderPlugin::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespersXAS" || scan->fileFormat() == "vespers2011XAS" || scan->fileFormat() == "vespers2011EXAFS")
		return true;

	return false;
}

bool VESPERS2011XASFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder)
{
	// Check for null scan reference.
	if (!scan)
		return false;

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	QFile file(sourceFileInfo.filePath());
	if(!file.open(QIODevice::ReadOnly)) {
		AMErrorMon::error(0, -1, "XASFileLoader parse error while loading scan data from file.");
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

	// Need to determine if the single element or four element vortex detector was used.  Also need to determine which ion chambers were used for I0 and It.
	// First two lines are useless.
	line = in.readLine();
	line = in.readLine();

	line = in.readLine();

	if (line.contains("#(2)"))
		line = in.readLine();

	bool usingSingleElement = line.contains("IOC1607-004");
	bool usingFourElement = line.contains("dxp1607-B21-04");

	if (usingSingleElement){

		data.resize(2048);

		foreach(QString afp, scan->additionalFilePaths())
			if(afp.contains("_spectra.dat"))
				spectra.setFileName(afp);

		if(spectra.fileName() == ""){

			// Needed until the non-trivial database upgrade happens so I can set all the additional filepaths.
			QString temp(sourceFileInfo.filePath());
			temp.chop(4);
			spectra.setFileName(temp+"_spectra.dat");
		}

		if(!spectra.open(QIODevice::ReadOnly)) {
			AMErrorMon::error(0, -1, QString("XASFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName()));
			return false;
		}
	}
	else if (usingFourElement){

		data.resize(2048);
		raw1.resize(2048);
		raw2.resize(2048);
		raw3.resize(2048);
		raw4.resize(2048);

		foreach(QString afp, scan->additionalFilePaths())
			if(afp.contains("_spectra.dat"))
				spectra.setFileName(afp);

		if(spectra.fileName() == ""){

			// Needed until the non-trivial database upgrade happens so I can set all the additional filepaths.
			QString temp(sourceFileInfo.filePath());
			temp.chop(4);
			spectra.setFileName(temp+"_spectra.dat");
		}

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

	while ((line = in.readLine()).contains("#")){
		//Do nothing
	}

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();

	// Some setup variables.
	int axisValueIndex = 0;

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
	else{

		for (int i = 0; i < scan->rawDataSourceCount(); i++)
			scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

	}
	while (!in.atEnd()){

		lineTokenized << line.split(", ");

		scan->rawData()->beginInsertRows(0);

		scan->rawData()->setAxisValue(0, axisValueIndex, lineTokenized.at(1).toDouble());

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
			for (int i = 0; i < scan->rawDataSourceCount()-1; i++)
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

		else{

			// In transmission, there is no 2D spectra.  Go through all the data sources.
			for (int i = 0; i < scan->rawDataSourceCount(); i++)
				scan->rawData()->setValue(axisValueIndex, i, AMnDIndex(), lineTokenized.at(i+2).toDouble());
		}

		scan->rawData()->endInsertRows();

		axisValueIndex++;
		line = in.readLine();
		lineTokenized.clear();
	}

	file.close();

	if (usingSingleElement || usingFourElement)
		spectra.close();
	return true;
}

bool VESPERS2011XASFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "vespersXAS" || scan->fileFormat() == "vespers2011XAS" || scan->fileFormat() == "vespers2011EXAFS");
}

Q_EXPORT_PLUGIN2(VESPERS2011XASFileLoaderFactory, VESPERS2011XASFileLoaderFactory)

