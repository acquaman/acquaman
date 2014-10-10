#include "CLSSR570Coordinator.h"

#include <QDebug>

#include "beamline/AMPVControl.h"

CLSSR570Coordinator::CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent)
	:QObject(parent)
{
	sensPutConnected_ = false;
	pvBaseName_ = pvBaseName;

	clsSR570_ = new CLSSR570(name, pvBaseName_, this);
	sr570CoordinatorControlSet = new AMControlSet(this);

	AMSinglePVControl* sensNumPVControl = new AMSinglePVControl(QString("%1:sens_num").arg(pvBaseName_), QString("%1:sens_num").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensNumDISAPVControl = new AMSinglePVControl(QString("%1:sens_num.DISA").arg(pvBaseName_), QString("%1:sens_num.DISA").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensNumDISVPVControl = new AMSinglePVControl(QString("%1:sens_num.DISV").arg(pvBaseName_), QString("%1:sens_num.DISV").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensUnitPVControl = new AMSinglePVControl(QString("%1:sens_unit").arg(pvBaseName_), QString("%1:sens_unit").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensUnitDISAPVControl = new AMSinglePVControl(QString("%1:sens_unit.DISA").arg(pvBaseName_), QString("%1:sens_unit.DISA").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensUnitDISVPVControl = new AMSinglePVControl(QString("%1:sens_unit.DISV").arg(pvBaseName_), QString("%1:sens_unit.DISV").arg(pvBaseName_), this, 0.5);

	sr570CoordinatorControlSet->addControl(sensNumPVControl);
	sr570CoordinatorControlSet->addControl(sensNumDISAPVControl);
	sr570CoordinatorControlSet->addControl(sensNumDISVPVControl);
	sr570CoordinatorControlSet->addControl(sensUnitPVControl);
	sr570CoordinatorControlSet->addControl(sensUnitDISAPVControl);
	sr570CoordinatorControlSet->addControl(sensUnitDISVPVControl);

	connect(clsSR570_, SIGNAL(isConnected(bool)), this, SLOT(onCLSSR570ConnectedChanged(bool)));
	connect(clsSR570_, SIGNAL(valueChanged()), this, SLOT(onCLSSR570ValueChanged()));
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
		qDebug() << QString("Coordinator(%1): sens_put is not connected!").arg(pvBaseName_);
		return;
	}
	if (!sr570CoordinatorControlSet->isConnected()) {
		qDebug() << QString("Coordinator (%1): At least one of the sensitivy PVs is not connected!").arg(pvBaseName_);
		return;
	}

	// update the value of sensitivity num pv and sensitivity unit pv
	AMControl * sensNumDISAControl = sr570CoordinatorControlSet->controlNamed(QString("%1:sens_num.DISA").arg(pvBaseName_));
	AMControl * sensNumDISVControl = sr570CoordinatorControlSet->controlNamed(QString("%1:sens_num.DISV").arg(pvBaseName_));
	AMControl * sensNumControl = sr570CoordinatorControlSet->controlNamed(QString("%1:sens_num").arg(pvBaseName_));
	double sensNumDISAValue = sensNumDISAControl->value();
	double sensNumDISVValue = sensNumDISVControl->value();

	sensNumDISAControl->move(sensNumDISVValue);
	sensNumControl->move(clsSR570_->sensNumSetpoint());
	sensNumDISAControl->move(sensNumDISAValue);

	AMControl * sensUnitDISAControl = sr570CoordinatorControlSet->controlNamed(QString("%1:sens_unit.DISA").arg(pvBaseName_));
	AMControl * sensUnitDISVControl = sr570CoordinatorControlSet->controlNamed(QString("%1:sens_unit.DISV").arg(pvBaseName_));
	AMControl * sensUnitControl = sr570CoordinatorControlSet->controlNamed(QString("%1:sens_unit").arg(pvBaseName_));
	double sensUnitDISAValue = sensUnitDISAControl->value();
	double sensUnitDISVValue = sensUnitDISVControl->value();
	sensUnitDISAControl->move(sensUnitDISVValue);
	sensUnitControl->move(clsSR570_->sensUnitSetpoint());
	sensUnitDISAControl->move(sensUnitDISAValue);
}

