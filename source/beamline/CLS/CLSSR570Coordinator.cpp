#include "CLSSR570Coordinator.h"

#include <QDebug>

CLSSR570Coordinator::CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent)
	:QObject(parent)
{
	pvBaseName_ = pvBaseName;
	clsSR570_ = new CLSSR570(name, pvBaseName);

	sensNumPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_num").arg(pvBaseName_), this, 0.5);
	sensNumDISAPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_num.DISA").arg(pvBaseName_), this, 0.5);
	sensNumDISVPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_num.DISV").arg(pvBaseName_), this, 0.5);
	sensUnitPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_unit").arg(pvBaseName_), this, 0.5);
	sensUnitDISAPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_unit.DISA").arg(pvBaseName_), this, 0.5);
	sensUnitDISVPVControl_ = new AMSinglePVControl(QString("%1-SR570-Sensitivity").arg(name), QString("%1:sens_unit.DISV").arg(pvBaseName_), this, 0.5);

	connect(clsSR570_, SIGNAL(isConnected(bool)), this, SLOT(onCLSSR570ConnectedChanged(bool)));
	connect(clsSR570_, SIGNAL(valueChanged()), this, SLOT(onCLSSR570ValueChanged()));

	connect(sensNumPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensNumControlConnectedChanged(bool)));
	connect(sensNumDISAPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensNumDISAControlConnectedChanged(bool)));
	connect(sensNumDISVPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensNumDISVControlConnectedChanged(bool)));
	connect(sensUnitPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensUnitControlConnectedChanged(bool)));
	connect(sensUnitDISAPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensUnitDISAControlConnectedChanged(bool)));
	connect(sensUnitDISVPVControl_, SIGNAL(connected(bool)), this, SLOT(onSensUnitDISVControlConnectedChanged(bool)));
}

CLSSR570Coordinator::~CLSSR570Coordinator()
{
}

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
	if (!sensNumDISAConnected_) {
		qDebug() << QString("%1:sens_num.DISA is not connected!").arg(pvBaseName_);
		return;
	}
	if (!sensNumDISVConnected_) {
		qDebug() << QString("%1:sens_num.DISV is not connected!").arg(pvBaseName_);
		return;
	}
	if (!sensUnitConnected_) {
		qDebug() << QString("%1:sens_unit is not connected!").arg(pvBaseName_);
		return;
	}
	if (!sensUnitDISAConnected_) {
		qDebug() << QString("%1:sens_unit.DISA is not connected!").arg(pvBaseName_);
		return;
	}
	if (!sensUnitDISVConnected_) {
		qDebug() << QString("%1:sens_unit.DISV is not connected!").arg(pvBaseName_);
		return;
	}

	// update the value of sensitivity num pv and sensitivity unit pv
	sensNumDISAPVControl_->move(sensNumDISVValue_);
	sensNumPVControl_->move(clsSR570_->sensNumSetpoint());
	sensNumDISAPVControl_->move(sensNumDISAValue_);

	sensUnitDISAPVControl_->move(sensUnitDISVValue_);
	sensUnitPVControl_->move(clsSR570_->sensUnitSetpoint());
	sensUnitDISAPVControl_->move(sensUnitDISAValue_);
}

void CLSSR570Coordinator::onSensNumControlConnectedChanged(bool connected)
{
	if(connected != sensNumConnected_){
		sensNumConnected_ = connected;
		if (sensNumConnected_)
			qDebug() << QString("%1:sens_num is connected!").arg(pvBaseName_);
	}
}

void CLSSR570Coordinator::onSensNumDISAControlConnectedChanged(bool connected)
{
	if(connected != sensNumDISAConnected_){
		sensNumDISAConnected_ = connected;
		if (sensNumDISAConnected_) {
			qDebug() << QString("%1:sens_num.DISA is connected!").arg(pvBaseName_);

			sensNumDISAValue_ = sensNumDISAPVControl_->value();
		}
	}
}

void CLSSR570Coordinator::onSensNumDISVControlConnectedChanged(bool connected)
{
	if(connected != sensNumDISVConnected_){
		sensNumDISVConnected_ = connected;
		if (sensNumDISVConnected_) {
			qDebug() << QString("%1:sens_num.DISV is connected!").arg(pvBaseName_);

			sensNumDISVValue_ = sensNumDISVPVControl_->value();
		}
	}
}

void CLSSR570Coordinator::onSensUnitControlConnectedChanged(bool connected)
{
	if(connected != sensUnitConnected_){
		sensUnitConnected_ = connected;
		if (sensUnitConnected_)
			qDebug() << QString("%1:sens_unit is connected!").arg(pvBaseName_);
	}
}

void CLSSR570Coordinator::onSensUnitDISAControlConnectedChanged(bool connected)
{
	if(connected != sensUnitDISAConnected_){
		sensUnitDISAConnected_ = connected;
		if (sensUnitDISAConnected_) {
			qDebug() << QString("%1:sens_unit.DISA is connected!").arg(pvBaseName_);

			sensUnitDISAValue_ = sensUnitDISAPVControl_->value();
		}
	}
}

void CLSSR570Coordinator::onSensUnitDISVControlConnectedChanged(bool connected)
{
	if(connected != sensUnitDISVConnected_){
		sensUnitDISVConnected_ = connected;
		if (sensUnitDISVConnected_) {
			qDebug() << QString("%1:sens_unit.DISV is connected!").arg(pvBaseName_);
			sensUnitDISVValue_ = sensUnitDISVPVControl_->value();
		}
	}
}

