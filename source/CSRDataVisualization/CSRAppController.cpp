#include "CSRAppController.h"

#include "CSRDataVisualization/CSRDataModel.h"

CSRAppController::CSRAppController(QObject *parent) :
	QObject(parent)
{
}
CSRAppController::~CSRAppController()
{
	mainWindow_->deleteLater();
}

bool CSRAppController::start()
{
	CSRDataModel *model = new CSRDataModel(this);
	mainWindow_ = new CSRMainWindow(model);
	mainWindow_->show();

	return true;
}
