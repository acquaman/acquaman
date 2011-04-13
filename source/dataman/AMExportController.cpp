#include "AMExportController.h"

#include <QComboBox>
#include <QHashIterator>
#include "acquaman.h"

#include "ui/AMExportWizard.h"

QHash<QString, AMExporterInfo> AMExportController::registeredExporters_;

AMExportController::AMExportController(const QList<QUrl>& scansToExport) :
	QObject()
{
	scansToExport_ = scansToExport;

	wizard_ = new AMExportWizard(this);
	wizard_->show();
}
