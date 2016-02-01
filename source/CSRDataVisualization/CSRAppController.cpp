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
	model->addFileConfiguration(new CSRFileConfiguration(735000000, 20000000, 877));
	model->addFileConfiguration(new CSRFileConfiguration(1500000000, 39999994, 1754));

	mainWindow_ = new CSRMainWindow(model);
	mainWindow_->show();

	return true;
}
