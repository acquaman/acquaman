#include "CLSSR570Coordinator.h"

#include <QDebug>

#include "beamline/AMPVControl.h"

CLSSR570Coordinator::CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent)
	:QObject(parent)
{
	pvUpdateStatus_ = Idle;

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

	connect(sensNumPVControl, SIGNAL(valueChanged(double)), this, SLOT(onSensNumValueChanged(double)));
	connect(sensNumDISAPVControl, SIGNAL(valueChanged(double)), this, SLOT(onSensNumDISAValueChanged(double)));
	connect(sensNumPROCPVControl, SIGNAL(valueChanged(double)), this, SLOT(onSensNumPROCValueChanged(double)));
	connect(sensUnitPVControl, SIGNAL(valueChanged(double)), this, SLOT(onSensUnitValueChanged(double)));
	connect(sensUnitDISAPVControl, SIGNAL(valueChanged(double)), this, SLOT(onSensUnitDISAValueChanged(double)));
	connect(sensUnitPROCPVControl, SIGNAL(valueChanged(double)), this, SLOT(onSensUnitPROCValueChanged(double)));

	connect(clsSR570_, SIGNAL(isConnected(bool)), this, SLOT(onCLSSR570ConnectedChanged(bool)));
	connect(clsSR570_, SIGNAL(valueChanged()), this, SLOT(onCLSSR570ValueChanged()));

	connect(sr570CoordinatorControlSet_, SIGNAL(connected(bool)), this, SLOT(onCLSSR570ControlsConnectedChanged(bool)));
}

void CLSSR570Coordinator::onCLSSR570ControlsConnectedChanged(bool connected)
{
	if(connected && !controlSetConnected_){
		if (sr570CoordinatorControlSet_->isConnected()) {
			qDebug() << QString("Coordinator (%1): All the sensitivity PVs are connected now!").arg(pvBaseName_);
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
	qDebug() << QString("Coordinator(%1): sens_put value changed (%2)!").arg(pvBaseName_).arg((clsSR570_->value()));

	pvUpdateStatus_ = SensPutValueChanged;

	AMControl * sensNumDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.DISA").arg(pvBaseName_));
	AMControl * sensNumDISVControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.DISV").arg(pvBaseName_));
	sensNumDISAValue_ = sensNumDISAControl->value();

	qDebug() << QString("Coordinator(%1): disable sens_num.DISA from %2 to %3!").arg(pvBaseName_).arg(sensNumDISAValue_).arg(sensNumDISVControl->value());
	pvUpdateStatusTransition(SensPutValueChanged, DisableSensNumDISA);
	sensNumDISAControl->move(sensNumDISVControl->value());

	AMControl * sensUnitDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.DISA").arg(pvBaseName_));
	AMControl * sensUnitDISVControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.DISV").arg(pvBaseName_));
	sensUnitDISAValue_ = sensUnitDISAControl->value();

	qDebug() << QString("Coordinator(%1): disable sens_unit.DISA from %2 to %3!").arg(pvBaseName_).arg(sensUnitDISAValue_).arg(sensUnitDISVControl->value());
	pvUpdateStatusTransition(SensPutValueChanged, DisableSensUnitDISA);
	sensUnitDISAControl->move(sensUnitDISVControl->value());
}

void CLSSR570Coordinator::onSensNumValueChanged(double)
{
	if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & UpdateSensNum)) {
		qDebug() << QString("Coordinator(%1): sens_num valueChanged ignored").arg(pvBaseName_);
		return;
	}

	qDebug() << QString("Coordinator(%1): enable sens_num.DISA!").arg(pvBaseName_);
	pvUpdateStatusTransition(UpdateSensNum, EnableSensNumDISA);

	AMControl * sensNumDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.DISA").arg(pvBaseName_));
	sensNumDISAControl->move(sensNumDISAValue_);
}

void CLSSR570Coordinator::onSensNumDISAValueChanged(double)
{
	AMControl * sensNumDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.DISA").arg(pvBaseName_));
	AMControl * sensNumDISVControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.DISV").arg(pvBaseName_));

	if (sensNumDISAControl->value() == sensNumDISVControl->value()) {
		if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & DisableSensNumDISA)) {
			qDebug() << QString("Coordinator(%1): sens_num.DISA valueChanged ignored (to update sens_num value)").arg(pvBaseName_);
			return;
		}

		AMControl * sensNumControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num").arg(pvBaseName_));
		qDebug() << QString("Coordinator(%1): move sens_num from %2 to %3!").arg(pvBaseName_).arg(sensNumControl->value()).arg(clsSR570_->sensNumSetpoint());
		pvUpdateStatusTransition(DisableSensNumDISA, UpdateSensNum);
		sensNumControl->move(clsSR570_->sensNumSetpoint());
	} else {
		if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & EnableSensNumDISA)) {
			qDebug() << QString("Coordinator(%1): sens_num.DISA valueChanged ignored (to update sens_num.PROC value)").arg(pvBaseName_);
			return;
		}

		qDebug() << QString("Coordinator(%1): force to enable sens_num.PROC!").arg(pvBaseName_);
		pvUpdateStatusTransition(EnableSensNumDISA, EnableSensNumPROC);
		AMControl * sensNumPROCControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_num.PROC").arg(pvBaseName_));
		sensNumPROCControl->move(1);
	}
}

void CLSSR570Coordinator::onSensNumPROCValueChanged(double)
{
	if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & EnableSensNumPROC)) {
		qDebug() << QString("Coordinator(%1): sens_num.PROC valueChanged ignored").arg(pvBaseName_);
		return;
	}

	qDebug() << QString("Coordinator(%1): sens_num.PROC value changed, restore status!").arg(pvBaseName_);
	pvUpdateStatusTransition(EnableSensNumPROC, Idle);
}


void CLSSR570Coordinator::onSensUnitValueChanged(double)
{
	if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & UpdateSensUnit)) {
		qDebug() << QString("Coordinator(%1): sens_unit valueChanged ignored").arg(pvBaseName_);
		return;
	}

	qDebug() << QString("Coordinator(%1): enable sens_unit.DISA!").arg(pvBaseName_);
	pvUpdateStatusTransition(UpdateSensUnit, EnableSensUnitDISA);
	AMControl * sensUnitDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.DISA").arg(pvBaseName_));
	sensUnitDISAControl->move(sensUnitDISAValue_);
}

void CLSSR570Coordinator::onSensUnitDISAValueChanged(double)
{
	AMControl * sensUnitDISAControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.DISA").arg(pvBaseName_));
	AMControl * sensUnitDISVControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.DISV").arg(pvBaseName_));

	if (sensUnitDISAControl->value() == sensUnitDISVControl->value()) {
		if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & DisableSensUnitDISA)) {
			qDebug() << QString("Coordinator(%1): sens_unit.DISA valueChanged ignored (to update sens_unit value)").arg(pvBaseName_);
			return;
		}

		AMControl * sensUnitControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit").arg(pvBaseName_));
		qDebug() << QString("Coordinator(%1): move sens_unit from %2 to %3!").arg(pvBaseName_).arg(sensUnitControl->value()).arg(clsSR570_->sensUnitSetpoint());
		pvUpdateStatusTransition(DisableSensUnitDISA, UpdateSensUnit);
		sensUnitControl->move(clsSR570_->sensUnitSetpoint());
	} else {
		if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & EnableSensUnitDISA)) {
			qDebug() << QString("Coordinator(%1): sens_unit.DISA valueChanged ignored (to update sens_unit.PROC value)").arg(pvBaseName_);
			return;
		}

		qDebug() << QString("Coordinator(%1): force to enable sens_unit.PROC!").arg(pvBaseName_);
		pvUpdateStatusTransition(EnableSensUnitDISA, EnableSensUnitPROC);
		AMControl * sensUnitPROCControl = sr570CoordinatorControlSet_->controlNamed(QString("%1:sens_unit.PROC").arg(pvBaseName_));
		sensUnitPROCControl->move(1);
	}
}

void CLSSR570Coordinator::onSensUnitPROCValueChanged(double)
{
	if (pvUpdateStatus_ == Idle || !(pvUpdateStatus_ & EnableSensUnitPROC)) {
		qDebug() << QString("Coordinator(%1): sens_unit.PROC valueChanged ignored").arg(pvBaseName_);
		return;
	}

	qDebug() << QString("Coordinator(%1): sens_unit.PROC value changed, restore status!").arg(pvBaseName_);
	pvUpdateStatusTransition(EnableSensUnitPROC, Idle);
}

void CLSSR570Coordinator::pvUpdateStatusTransition(int from, int to)
{
	pvUpdateStatus_ ^= from;

	if (to == Idle) {
		if (pvUpdateStatus_ == SensPutValueChanged) {
			pvUpdateStatus_ = Idle;
			qDebug() << QString("Coordinator(%1): Mission Accomplished!").arg(pvBaseName_);
		}
	} else {
		pvUpdateStatus_ |= to;
	}
}

