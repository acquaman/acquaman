#include "CLSSR570Coordinator.h"

#include <QDebug>

CLSSR570Coordinator::CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent)
	:QObject(parent)
{
	pvBaseName_ = pvBaseName;
	clsSR570_ = new CLSSR570(name, pvBaseName);

//	sensPutPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_put").arg(pvBaseName_), this, 0.5);
	sensNumPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_num").arg(pvBaseName_), this, 0.5);
	sensUnitPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_unit").arg(pvBaseName_), this, 0.5);

	connect(clsSR570_, SIGNAL(isConnected(bool)), this, SLOT(onCLSSR570ConnectedChanged(bool)));
	connect(clsSR570_, SIGNAL(valueChanged()), this, SLOT(onCLSSR570ValueChanged()));

	connect(sensNumPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensNumControlConnectedChanged(bool)));
	connect(sensUnitPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensUnitControlConnectedChanged(bool)));
}

CLSSR570Coordinator::~CLSSR570Coordinator()
{
}

//============= the followings are the protected slot funcitons ============
void CLSSR570Coordinator::onCLSSR570ConnectedChanged(bool connected)
{
	if(connected != sensPutConnected_){
		sensPutConnected_ = connected;
	}
}

void CLSSR570Coordinator::onCLSSR570ValueChanged()
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
	sensNumPVControl_->move(clsSR570_->sensNumSetpoint());
	sensNumPVControl_->enableProcessRecord();

	sensUnitPVControl_->disableProcessRecord();
	sensUnitPVControl_->move(clsSR570_->sensUnitSetpoint());
	sensUnitPVControl_->enableProcessRecord();
}

void CLSSR570Coordinator::onSensNumControlConnectedChanged(bool connected)
{
	if(connected != sensNumConnected_){
		sensNumConnected_ = connected;
	}
}

void CLSSR570Coordinator::onSensUnitControlConnectedChanged(bool connected)
{
	if(connected != sensUnitConnected_){
		sensUnitConnected_ = connected;
	}
}

