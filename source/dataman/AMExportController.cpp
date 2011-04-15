#include "AMExportController.h"

#include <QComboBox>
#include <QHashIterator>
#include <QDir>
#include "acquaman.h"

#include "dataman/AMExporter.h"
#include "dataman/AMUser.h"


#include <QDebug>

QHash<QString, AMExporterInfo> AMExportController::registeredExporters_;

AMExportController::AMExportController(const QList<QUrl>& scansToExport) :
	QObject()
{
	exporter_ = 0;
	scansToExport_ = scansToExport;

	destinationFolderPath_ = AMUser::user()->lastExportDestination();
	if(destinationFolderPath_.isEmpty())
		destinationFolderPath_ = QDir::toNativeSeparators(QDir::homePath());
}

bool AMExportController::chooseExporter(const QString &exporterClassName)
{
	if(!registeredExporters_.contains(exporterClassName))
		return false;

	if(exporter_)
		delete exporter_;

	exporter_ = qobject_cast<AMExporter*>(registeredExporters_.value(exporterClassName).metaObject->newInstance());
	exporter_->setParent(this);

	return true;
}
