#include "VESPERS20122DFileLoaderPlugin.h"

#include <QtGui>

#include "util/AMErrorMonitor.h"
#include "analysis/AM1DExpressionAB.h"
#include "dataman/AMScan.h"

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
	/// \todo implement load.
	return true;
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

