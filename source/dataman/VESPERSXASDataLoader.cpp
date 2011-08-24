#include "VESPERSXASDataLoader.h"

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"

#include <QFile>
#include <QTextStream>

VESPERSXASDataLoader::VESPERSXASDataLoader(AMXASScan *scan)
	: AMAbstractFileLoader(scan)
{
}

bool VESPERSXASDataLoader::loadFromFile(const QString &filepath, bool setMetaData, bool setRawDataSources, bool createDefaultAnalysisBlocks)
{
	// Currently don't have meta data.
	Q_UNUSED(setMetaData)
	Q_UNUSED(setRawDataSources)
	Q_UNUSED(createDefaultAnalysisBlocks)

	AMXASScan *scan = qobject_cast<AMXASScan *>(scan_);

	if (!scan){

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "VESPERS XAS File Loader: Could not load XAS data into a non-XAS scan."));
		return false;
	}

	/*VESPERSXASScanConfiguration *config = qobject_cast<VESPERSXASScanConfiguration *>(scan->scanConfiguration());

	if (!config){

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, 0, "VESPERS XAS File Loader: Scan does not have a valid scan configuration."));
		return false;
	}*/

	QFile file(filepath);
	if(!file.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "XASFileLoader parse error while loading scan data from file."));
		return false;
	}

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;
	QVector<double> data;

	// Don't care about the info in the header.  All of the important stuff should already be in the database.
	while((line = in.readLine()).contains("#"));

	// Clear any old data so we can start fresh.
	scan->clearRawDataPointsAndMeasurements();

	// Some setup variables.
	int axisValueIndex = 0;

	/*switch(config->fluorescenceDetectorChoice()){

	case VESPERSXASScanConfiguration::None:

		scan_->rawData()->addMeasurement(AMMeasurementInfo("I0", config->ionChamberName(config->incomingChoice())));
		scan_->rawData()->addMeasurement(AMMeasurementInfo("It", config->ionChamberName(config->transmissionChoice())));

		while (!in.atEnd()){

			line = in.readLine();
			lineTokenized << line.split(", ");

			scan->rawData()->beginInsertRows(0);

			scan_->rawData()->setAxisValue(0, axisValueIndex, lineTokenized.at(1).toDouble());
			scan_->rawData()->setValue(axisValueIndex, 0, AMnDIndex(), lineTokenized.at(2).toDouble());
			scan_->rawData()->setValue(axisValueIndex, 1, AMnDIndex(), lineTokenized.at(3).toDouble());

			scan->rawData()->endInsertRows();

			axisValueIndex++;
		}

		break;

	case VESPERSXASScanConfiguration::SingleElement:
		break;

	case VESPERSXASScanConfiguration::FourElement:
		break;

	default:
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "XASFileLoader parse error while loading scan data from file. Invalid configuration."));
		return false;
	}

	AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
	transmission->setDescription("Transmission");
	transmission->setInputDataSources(QList<AMDataSource *>() << scan->rawDataSources()->at(0) << scan->rawDataSources()->at(1));
	transmission->setExpression(QString("ln(%1/%2)").arg(scan->rawDataSources()->at(0)->name()).arg(scan->rawDataSources()->at(1)->name()));

	scan->addAnalyzedDataSource(transmission);
*/
	return true;
}
