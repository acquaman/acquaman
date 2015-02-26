#include "CSRAppController.h"

#include "CSRDataVisualization/CSRMainWindow.h"
#include "CSRDataVisualization/CSRDataModel.h"

CSRAppController::CSRAppController(QObject *parent) :
	QObject(parent)
{
}
CSRAppController::~CSRAppController()
{

}

bool CSRAppController::start()
{
	CSRDataModel *model = new CSRDataModel(this);
	model->loadDataFile("/home/hunterd/beamline/programming/CSR/CSR-50FBH-L-141208_235004_Ch1.txt");
	model->computeMeanAndStandardDeviation();

	CSRMainWindow *mainWindow = new CSRMainWindow(model);
	mainWindow->show();

	return true;
}
