#include "CLSVariableIntegrationTime.h"

CLSVariableIntegrationTime::CLSVariableIntegrationTime(const QString &baseName, QObject *parent)
	: QObject(parent)
{
	mode_ = new AMSinglePVControl("Mode", baseName + ":enable", this, 0.1);
	defaultTime_ = new AMSinglePVControl("Default Time", baseName + ":default", this, 0.1);
	threshold_ = new AMSinglePVControl("Threshold", baseName + ":trigger:thresh", this, 0.1);
	function_ = new AMSinglePVControl("Function", baseName + ":method", this, 0.1);
	lowVal_ = new AMSinglePVControl("Low Value", baseName + ":xmin", this, 0.1);
	highVal_ = new AMSinglePVControl("High Value", baseName + ":xmax", this, 0.1);
	maxTime_ = new AMSinglePVControl("Maximum Time", baseName + ":ymax", this, 0.1);
	compute_ = new AMSinglePVControl("Compute", baseName + ":aN:calc:run", this, 0.1);
}
