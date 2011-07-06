#include "VESPERSXRFDataLoader.h"
#include "util/AMErrorMonitor.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "analysis/AMDeadTimeAB.h"
#include "analysis/AM1DSummingAB.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

VESPERSXRFDataLoader::VESPERSXRFDataLoader(AMXRFScan *scan)
	: AMAbstractFileLoader(scan)
{
}

bool VESPERSXRFDataLoader::loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks)
{
	// Currently don't have meta data.
	Q_UNUSED(setMetaData)
	Q_UNUSED(setRawDataSources)
	Q_UNUSED(createDefaultAnalysisBlocks)

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

	int elements = config->detectorInfo().elements();

	for (int i = 0; i < scan->rawDataSources()->count(); i++){

		if (i < elements){

			line = in.readLine();
			lineTokenized = line.split(",");
			data.resize(lineTokenized.size());
			for (int j = 0; j < lineTokenized.size(); j++)
				data[j] = lineTokenized.at(j).toInt();

			scan->rawData()->addMeasurement(AMMeasurementInfo(QString("Element %1").arg(i+1), QString("Element %1").arg(i+1), "eV", config->detectorInfo().axes()));
			scan->rawData()->setValue(AMnDIndex(), i, data.constData(), data.size());
		}
		else{

			line = in.readLine();
			if (i >= elements && i < 2*elements)
				scan->rawData()->addMeasurement(AMMeasurementInfo(QString("icr%1").arg(i+1), QString("Input count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));
			else
				scan_->rawData()->addMeasurement(AMMeasurementInfo(QString("ocr%1").arg(i+1), QString("Output count rate %1").arg(i+1), "%", QList<AMAxisInfo>()));
			scan->rawData()->setValue(AMnDIndex(), i, AMnDIndex(), line.toDouble());
		}
	}

	file.close();

	for (int i = 0; i < elements; i++){

		AMDeadTimeAB *temp = (AMDeadTimeAB *)scan->analyzedDataSources()->at(i);
		temp->setInputDataSourcesImplementation(QList<AMDataSource *>() << (AMDataSource *)scan->rawDataSources()->at(i) << (AMDataSource *)scan->rawDataSources()->at(i+elements) << (AMDataSource *)scan->rawDataSources()->at(i+2*elements));
	}

	if (elements > 1){

		AM1DSummingAB *corr = (AM1DSummingAB *)scan->analyzedDataSources()->at(scan->analyzedDataSourceCount()-1);
		QList<AMDataSource *> list;
		for (int i = 0; i < scan->analyzedDataSourceCount()-1; i++)
			list << (AMDataSource *)scan->analyzedDataSources()->at(i);
		corr->setInputDataSourcesImplementation(list);
	}

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

	// There are two types of raw data sources.  The spectra of rank 1 and the dead time of rank 0.
	for (int i = 0; i < scan->rawDataSources()->count(); i++){

		if (scan->rawDataSources()->at(i)->rank() == 1){

			qint32 counts = scan->rawData()->value(AMnDIndex(), i, AMnDIndex(0));
			out << counts;

			for (int j = 1; j < scan->rawData()->measurementAt(i).size(0); j++){

				qint32 counts = scan->rawData()->value(AMnDIndex(), i, AMnDIndex(j));
				out << "," << counts;
			}
		}
		else {

			double counts = scan->rawData()->value(AMnDIndex(), i, AMnDIndex());
			out << counts;
		}

		out << "\n";
	}

	file.close();

	return true;
}
