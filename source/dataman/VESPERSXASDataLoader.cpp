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

	QFile file(filepath);
	if(!file.open(QIODevice::ReadOnly)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "XASFileLoader parse error while loading scan data from file."));
		return false;
	}

	QTextStream in(&file);
	QString line;
	QStringList lineTokenized;
	QVector<double> data;

	// Need to determine if the single element or four element vortex detector was used.  Also need to determine which ion chambers were used for I0 and It.
	// First two lines are useless.
	line = in.readLine();
	line = in.readLine();

	line = in.readLine();

	if (line.contains("#(2)"))
		line = in.readLine();

	bool usingSingleEl = line.contains("IOC1607-004");
	bool usingFourEl = line.contains("dxp1607-B21-04");

	lineTokenized = line.split(" ");

	QString i0(lineTokenized.at(3));
	QString it(lineTokenized.at(4));

	lineTokenized.clear();

	if (i0.contains("Isplit"))
		i0 = "Isplit";
	else if (i0.contains("mcs07"))
		i0 = "Iprekb";
	else if (i0.contains("mcs08"))
		i0 = "Imini";
	else
		return false;

	if (it.contains("mcs07"))
		it = "Iprekb";
	else if (it.contains("mcs08"))
		it = "Imini";
	else if (it.contains("mcs09"))
		it = "Ipost";
	else
		return false;

	while ((line = in.readLine()).contains("#"));

	// Clear any old data so we can start fresh.
	scan->clearRawDataPoints();

	// Some setup variables.
	int axisValueIndex = 0;

	// If both are false, then we are in pure transmission mode.
	if (!usingSingleEl && !usingFourEl){

		//scan_->rawData()->addMeasurement(AMMeasurementInfo("I0", i0));
		//scan_->rawData()->addMeasurement(AMMeasurementInfo("It", it));

		while (!in.atEnd()){

			lineTokenized << line.split(", ");

			scan->rawData()->beginInsertRows(0);

			scan_->rawData()->setAxisValue(0, axisValueIndex, lineTokenized.at(1).toDouble());
			scan_->rawData()->setValue(axisValueIndex, 0, AMnDIndex(), lineTokenized.at(2).toDouble());
			scan_->rawData()->setValue(axisValueIndex, 1, AMnDIndex(), lineTokenized.at(3).toDouble());

			scan->rawData()->endInsertRows();

			axisValueIndex++;
			line = in.readLine();
			lineTokenized.clear();
		}
	}

	else if (usingSingleEl){

	}

	else if (usingFourEl){

	}

	else{

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -1, "XASFileLoader parse error while loading scan data from file. Invalid configuration."));
		return false;
	}

	/*AM1DExpressionAB* transmission = new AM1DExpressionAB("trans");
	transmission->setDescription("Transmission");
	transmission->setInputDataSources(QList<AMDataSource *>() << scan->rawDataSources()->at(0) << scan->rawDataSources()->at(1));
	transmission->setExpression(QString("ln(%1/%2)").arg(scan->rawDataSources()->at(0)->name()).arg(scan->rawDataSources()->at(1)->name()));

	scan->addAnalyzedDataSource(transmission);
*/
	return true;
}
