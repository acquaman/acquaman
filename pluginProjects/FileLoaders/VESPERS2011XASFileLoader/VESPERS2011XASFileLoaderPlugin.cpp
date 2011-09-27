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
	if (scan->fileFormat() == "vespersXAS" || scan->fileFormat() == "vespers2011XAS")
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
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "XASFileLoader parse error while loading scan data from file."));
		return false;
	}

	QFile spectra;
	QVector<int> data;

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

	bool usingVortex = line.contains("IOC1607-004") || line.contains("dxp1607-B21-04");

	if (usingVortex){

		data.resize(2048);
		QString temp(sourceFileInfo.filePath());
		temp.chop(4);
		spectra.setFileName(temp+"_spectra.dat");

		if(!spectra.open(QIODevice::ReadOnly)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, QString("XASFileLoader parse error while loading scan spectra data from %1.").arg(spectra.fileName())));
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

	if (usingVortex){

		for (int i = 0; i < scan->rawDataSourceCount()-1; i++)
			scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(i)->name(), scan->rawDataSources()->at(i)->description()));

		QList<AMAxisInfo> axisInfo;
		AMAxisInfo ai("Energy", 2048, "Energy", "eV");
		ai.increment = 10;
		ai.start = AMNumber(0);
		ai.isUniform = true;
		axisInfo << ai;

		scan->rawData()->addMeasurement(AMMeasurementInfo(scan->rawDataSources()->at(scan->rawDataSourceCount()-1)->name(), scan->rawDataSources()->at(scan->rawDataSourceCount()-1)->description(), "eV", axisInfo));
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
		if (usingVortex){

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

	if (usingVortex)
		spectra.close();

	return true;
}

Q_EXPORT_PLUGIN2(VESPERS2011XASFileLoaderPlugin, VESPERS2011XASFileLoaderPlugin)
