#include "XRDAnalysisAppController.h"

#include "ui/XRDAnalysis/XRDAnalysisMainWindow.h"

XRDAnalysisAppController::XRDAnalysisAppController(QObject *parent)
	: QObject(parent)
{
	mainWindow_ = new XRDAnalysisMainWindow;
	mainWindow_->show();
}
