#include "CLSSR570Coordinator.h"

CLSSR570Coordinator::CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent) :
    QObject(parent)
{
	pvBaseName_ = pvBaseName;

//	sensitivityPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sensitivity").arg(pvBaseName_), this, 0.5);
	sensPutPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_put").arg(pvBaseName_), this, 0.5);
	sensNumPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_num").arg(pvBaseName_), this, 0.5);
	sensUnitPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_unit").arg(pvBaseName_), this, 0.5);

//	connect(sensitivityPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensitivityControlConnectedChanged(bool)));
//	connect(sensitivityPVControl_, SIGNAL(valueChanged(double)), this, SLOT(onSensitivityControlValueChanged(double)));
	connect(sensPutPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensPutControlConnectedChanged(bool)));
	connect(sensPutPVControl_, SIGNAL(valueChanged(double)), this, SLOT(onSensPutControlValueChanged(double)));

	connect(sensNumPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensNumControlConnectedChanged(bool)));
	connect(sensUnitPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensUnitControlConnectedChanged(bool)));
}

CLSSR570Coordinator::~CLSSR570Coordinator()
{
}

//============= the followings are the protected slot funcitons ============
void CLSSR570Coordinator::onSensitivityControlConnectedChanged(bool connected)
{
}

void CLSSR570Coordinator::onSensitivityControlValueChanged(double)
{

}

void CLSSR570Coordinator::onSensPutControlConnectedChanged(bool connected)
{
	if(connected != sensPutConnected_){
		sensPutConnected_ = connected;
	}
}

void CLSSR570Coordinator::onSensPutControlValueChanged(double value)
{
	if (!sensPutConnected_) {
		qDebug() << QString("%1:sens_put is not connected!").arg(pvBaseName_);
		return;
	}
	if (!sensNumConnected_) {
		qDebug() << QString("%1:sens_num is not connected!").arg(pvBaseName_);
		return;
	}
	if (!sensUnitConnected_) {
		qDebug() << QString("%1:sens_unit is not connected!").arg(pvBaseName_);
		return;
	}

	// update the value of sensitivity num pv and sensitivity unit pv
	sensNumPVControl_->disableProcessRecord();
	sensNumPVControl_->move(sensitivityNums_.indexOf(value()));
	sensNumPVControl_->enableProcessRecord();

	sensUnitPVControl_->disableProcessRecord();
	sensUnitPVControl_->move(sensitivityUnits_.indexOf(units()));
	sensUnitPVControl_->enableProcessRecord();
}

void CLSSR570Coordinator::onSensNumControlConnectedChanged(bool connected)
{
	if(connected != sensPutConnected_){
		sensNumConnected_ = connected;
	}
}

void CLSSR570Coordinator::onSensUnitControlConnectedChanged(bool connected)
{
	if(connected != sensPutConnected_){
		sensUnitConnected_ = connected;
	}
}

