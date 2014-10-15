#include "CLSSR570Coordinator.h"

#include <QDebug>

#include "beamline/AMPVControl.h"

CLSSR570Coordinator::CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent)
	:QObject(parent)
{
	sensPutConnected_ = false;
	controlSetConnected_ = false;
	pvBaseName_ = pvBaseName;

	clsSR570_ = new CLSSR570(name, pvBaseName_, this);
	sr570CoordinatorControlSet_ = new AMControlSet(this);

	AMSinglePVControl* sensNumPVControl = new AMSinglePVControl(QString("%1:sens_num").arg(pvBaseName_), QString("%1:sens_num").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensNumDISAPVControl = new AMSinglePVControl(QString("%1:sens_num.DISA").arg(pvBaseName_), QString("%1:sens_num.DISA").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensNumDISVPVControl = new AMSinglePVControl(QString("%1:sens_num.DISV").arg(pvBaseName_), QString("%1:sens_num.DISV").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensNumPROCPVControl = new AMSinglePVControl(QString("%1:sens_num.PROC").arg(pvBaseName_), QString("%1:sens_num.PROC").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensUnitPVControl = new AMSinglePVControl(QString("%1:sens_unit").arg(pvBaseName_), QString("%1:sens_unit").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensUnitDISAPVControl = new AMSinglePVControl(QString("%1:sens_unit.DISA").arg(pvBaseName_), QString("%1:sens_unit.DISA").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensUnitDISVPVControl = new AMSinglePVControl(QString("%1:sens_unit.DISV").arg(pvBaseName_), QString("%1:sens_unit.DISV").arg(pvBaseName_), this, 0.5);
	AMSinglePVControl* sensUnitPROCPVControl = new AMSinglePVControl(QString("%1:sens_unit.PROC").arg(pvBaseName_), QString("%1:sens_unit.PROC").arg(pvBaseName_), this, 0.5);

	sr570CoordinatorControlSet_->addControl(sensNumPVControl);
	sr570CoordinatorControlSet_->addControl(sensNumDISAPVControl);
	sr570CoordinatorControlSet_->addControl(sensNumDISVPVControl);
	sr570CoordinatorControlSet_->addControl(sensNumPROCPVControl);
	sr570CoordinatorControlSet_->addControl(sensUnitPVControl);
	sr570CoordinatorControlSet_->addControl(sensUnitDISAPVControl);
	sr570CoordinatorControlSet_->addControl(sensUnitDISVPVControl);
	sr570CoordinatorControlSet_->addControl(sensUnitPROCPVControl);

	connect(clsSR570_, SIGNAL(isConnected(bool)), this, SLOT(onCLSSR570ConnectedChanged(bool)));
	connect(clsSR570_, SIGNAL(valueChanged()), this, SLOT(onCLSSR570ValueChanged()));

	connect(sr570CoordinatorControlSet_, SIGNAL(connected(bool)), this, SLOT(onCLSSR570ControlsConnectedChanged(bool)));
}

CLSSR570Coordinator::~CLSSR570Coordinator()
{
}

void CLSSR570Coordinator::onCLSSR570ControlsConnectedChanged(bool connected)
{
	if(connected && !controlSetConnected_){
		if (sr570CoordinatorControlSet_->isConnected()) {
			qDebug() << QString("Coordinator (%1): All the sensitivy PVs are connected now!").arg(pvBaseName_);
			controlSetConnected_ = sr570CoordinatorControlSet_->isConnected();
			onCLSSR570ValueChanged();
		}
	}
}

void CLSSR570Coordinator::onCLSSR570ConnectedChanged(bool connected)
{
	if(connected != sensPutConnected_){
		sensPutConnected_ = connected;
		if (sensPutConnected_) {
			qDebug() << QString("Coordinator(%1): sens_put is connected!").arg(pvBaseName_);
			onCLSSR570ValueChanged();
		}
	}
}

void CLSSR570Coordinator::onCLSSR570ValueChanged()
{
	if (!sensPutConnected_ || !controlSetConnected_) {
		qDebug() << QString("Coordinator(%1): at least one of the required PVs is not connected.").arg(pvBaseName_);
		return;
	}

	qDebug() << "\n\n";
	qDebug() << QString("Coordinator(%1): ------").arg(pvBaseName_);
	qDebug() << QString("Coordinator(%1): sens_put value changed!").arg(pvBaseName_);

	// update the value of sensitivity num pv and sensitivity unit pv
	AMControl * sensNumControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num").arg(pvBaseName_));
	AMControl * sensNumDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.DISA").arg(pvBaseName_));
	AMControl * sensNumDISVControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.DISV").arg(pvBaseName_));
	AMControl * sensNumPROCControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.PROC").arg(pvBaseName_));
	double sensNumDISAValue = sensNumDISAControl->value();
	double sensNumDISVValue = sensNumDISVControl->value();

	qDebug() << QString("Coordinator(%1): disable sens_num.DISA!").arg(pvBaseName_);
	sensNumDISAControl->move(sensNumDISVValue);
	qDebug() << QString("Coordinator(%1): move sens_num from %2 to %3!").arg(pvBaseName_).arg(sensNumControl->value()).arg(clsSR570_->sensNumSetpoint());
	sensNumControl->move(clsSR570_->sensNumSetpoint());
	qDebug() << QString("Coordinator(%1): enable sens_num.DISA!").arg(pvBaseName_);
	sensNumDISAControl->move(sensNumDISAValue);
	qDebug() << QString("Coordinator(%1): force to enable sens_num.PROC!").arg(pvBaseName_);
	sensNumPROCControl->move(1);


	AMControl * sensUnitControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit").arg(pvBaseName_));
	AMControl * sensUnitDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.DISA").arg(pvBaseName_));
	AMControl * sensUnitDISVControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.DISV").arg(pvBaseName_));
	AMControl * sensUnitPROCControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.PROC").arg(pvBaseName_));
	double sensUnitDISAValue = sensUnitDISAControl->value();
	double sensUnitDISVValue = sensUnitDISVControl->value();
	qDebug() << QString("Coordinator(%1): disable sens_unit.DISA!").arg(pvBaseName_);
	sensUnitDISAControl->move(sensUnitDISVValue);
	qDebug() << QString("Coordinator(%1): move sens_unit from %2 to %3!").arg(pvBaseName_).arg(sensUnitControl->value()).arg(clsSR570_->sensUnitSetpoint());
	sensUnitControl->move(clsSR570_->sensUnitSetpoint());
	qDebug() << QString("Coordinator(%1): enable sens_unit.DISA!").arg(pvBaseName_);
	sensUnitDISAControl->move(sensUnitDISAValue);
	qDebug() << QString("Coordinator(%1): force to enable sens_unit.PROC!").arg(pvBaseName_);
	sensUnitPROCControl->move(1);

	qDebug() << QString("Coordinator(%1): Finished updating sens_num/sens_unit!").arg(pvBaseName_);
}

