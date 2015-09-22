#include "VESPERS2011XRFFileLoaderPlugin.h"

#include "AMQtGui.h"

#include "util/AMErrorMonitor.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"
#include "dataman/AMScan.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

bool VESPERS2011XRFFileLoaderPlugin::accepts(AMScan *scan)
{
	if (scan->fileFormat() == "vespersXRF" || scan->fileFormat() == "vespers2011XRF")
		return true;

	return false;
}

bool VESPERS2011XRFFileLoaderPlugin::load(AMScan *scan, const QString &userDataFolder, AMErrorMon *errorMonitor)
{
	// Check for null scan reference.
	if (!scan)
		return false;

	QFileInfo sourceFileInfo(scan->filePath());
	if(sourceFileInfo.isRelative())
		sourceFileInfo.setFile(userDataFolder + "/" + scan->filePath());

	QFile file(sourceFileInfo.filePath());
	if(!file.open(QIODevice::ReadOnly)) {
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Serious, VESPERS2011XRFFILELOADERPLUGIN_CANNOT_OPEN_FILE, "XRFFileLoader parse error while loading scan data from file."));
		return false;
	}

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;
	QVector<int> data;

	// Clear any old data so we can start fresh.
	scan->clearRawDataPoints();

	// Single element detector has 3 data sources, four element has 12.  Any other number is no good.
	int elements;
	// SXRMB bruker has one data source and differnt Axis setting
	bool SXRMBBruker = false;
	bool bioXASXRFDetector = false;

	if (scan->rawDataSourceCount() == 1) {
		elements = 1;
		SXRMBBruker = true;
	} else if (scan->rawDataSourceCount() == 3)
		elements = 1;
	else if (scan->rawDataSourceCount() == 4){
		elements = 4;
		bioXASXRFDetector = true;
	}
	else if (scan->rawDataSourceCount() == 12)
		elements = 4;
	else if (scan->rawDataSourceCount() == 20) {
		elements = 20;
		bioXASXRFDetector = true;
	}
	else if (scan->rawDataSourceCount() == 32){
		elements = 32;
		bioXASXRFDetector = true;
	}
	else{
		errorMonitor->exteriorReport(AMErrorReport(0, AMErrorReport::Alert, VESPERS2011XRFFILELOADERPLUGIN_UNRECOGNIZED_FILE_TYPE, "XRFFileLoader cannot recognize file."));
		return false;
	}

	for (int i = 0; i < scan->rawDataSources()->count(); i++){

		line = in.readLine();

		if (i < elements){

			lineTokenized = line.split(",");
			data.resize(lineTokenized.size());
			for (int j = 0; j < lineTokenized.size(); j++)
				data[j] = lineTokenized.at(j).toInt();

			// Note!  Not general!
			QList<AMAxisInfo> axisInfo;
			AMAxisInfo ai("Energy", 2048, "Energy", "eV");

			if (bioXASXRFDetector)
				ai.size = 4096;

			if (SXRMBBruker) {
				ai.increment = 5.0;
				ai.start = AMNumber(-450);
			} else {
				ai.increment = 10;
				ai.start = AMNumber(0);
			}
			ai.isUniform = true;
			axisInfo << ai;

			scan->rawData()->addMeasurement(AMMeasurementInfo(QString("Element %1").arg(i+1), QString("Element %1").arg(i+1), "eV", axisInfo));
			scan->rawData()->setValue(AMnDIndex(), i, data.constData());
		}
		else{

			if (i >= elements && i < 2*elements)
				scan->rawData()->addMeasurement(AMMeasurementInfo(QString("icr%1").arg(i+1), QString("Input count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));
			else
				scan->rawData()->addMeasurement(AMMeasurementInfo(QString("ocr%1").arg(i+1), QString("Output count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));

			scan->rawData()->setValue(AMnDIndex(), i, AMnDIndex(), line.toDouble());
		}
	}

	file.close();

	return true;
}

bool VESPERS2011XRFFileLoaderFactory::accepts(AMScan *scan)
{
	return (scan->fileFormat() == "vespersXRF" || scan->fileFormat() == "vespers2011XRF");
}

Q_EXPORT_PLUGIN2(VESPERS2011XRFFileLoaderFactory, VESPERS2011XRFFileLoaderFactory)

