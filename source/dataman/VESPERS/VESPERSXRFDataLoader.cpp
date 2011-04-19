#include "VESPERSXRFDataLoader.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

VESPERSXRFDataLoader::VESPERSXRFDataLoader(AMXRFScan *scan)
	: AMAbstractFileLoader(scan)
{
}

bool VESPERSXRFDataLoader::loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks)
{
	AMXRFScan *scan = qobject_cast<AMXRFScan *>(scan_);

	if (!scan){

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "VESPERS XRF File Loader: Could not load XRF data into a non-XRF scan."));
		return false;
	}

	VESPERSXRFScanConfiguration *config = qobject_cast<VESPERSXRFScanConfiguration *>(scan->scanConfiguration());

	if (!config){

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "VESPERS XRF File Loader: Scan does not have a valid scan configuration."));
		return false;
	}

	QFile file(filepath);
	if(!file.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "XRFFileLoader parse error while loading scan data from file."));
		return false;
	}

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;
	QVector<int> data;

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();

	qDebug() << "Number of raw data sources: " << scan->rawDataSources()->count();

	for (int i = 0; i < scan->rawDataSources()->count(); i++){

		line = in.readLine();
		lineTokenized = line.split(",");
		data.resize(lineTokenized.size());
		for (int j = 0; j < lineTokenized.size(); j++)
			data[j] = lineTokenized.at(j).toInt();

		/// \todo This shouldn't use the current detector settings.  Use the saved ones...  need to save them first though.
		scan->rawData()->addMeasurement(AMMeasurementInfo(QString("Element %1").arg(i+1), QString("Element %1").arg(i+1), "eV", config->detectorInfo().axes()));
		scan->rawData()->setValue(AMnDIndex(), i, data.constData(), data.size());
	}

	file.close();

	// Currently don't have meta data, raw data or analysis blocks.
	Q_UNUSED(setMetaData)
	Q_UNUSED(setRawDataSources)
	Q_UNUSED(createDefaultAnalysisBlocks)

	return true;
}

bool VESPERSXRFDataLoader::saveToFile(const QString &filepath)
{
	AMXRFScan *scan = qobject_cast<AMXRFScan *>(scan_);

	QFile file(filepath);
	if(!file.open(QIODevice::WriteOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "XRFFileLoader parse error while savings scan data from file."));
		return false;
	}

	QTextStream out(&file);

	for (int i = 0; i < scan->rawDataSources()->count(); i++){

		qint32 counts = scan->rawData()->value(AMnDIndex(), i, AMnDIndex(0));
		out << counts;
		for (int j = 1; j < scan->rawData()->measurementAt(i).size(0); j++){

			qint32 counts = scan->rawData()->value(AMnDIndex(), i, AMnDIndex(j));
			out << "," << counts;
		}
		out << "\n";
	}

	file.close();

	return true;
}
