/*
 * For SR570, when the value of sens_put PV is changed, the values of
 * sens_num and sens_unit are not updated accordingly.
 * This CLSSR570Coordinator is designed to update sens_num/sens_unit
 * when the value change of sens_put is changed.To reach this goal,
 * we have to disable the sens_num/sens_unit PVs before we write the value.
 * Otherwise, the write operation will trigger an update of sens_put PV.
 *
 * To disable a PV, we should set .DISA and .DISV same value;
 * To enable a PV, we should set .DISA and .DISV differt values and put a non-zero value to .PROC
 *
 */

#ifndef CLSSR570COORDINATOR_H
#define CLSSR570COORDINATOR_H

#include <QObject>

#include "beamline/AMControlSet.h"
#include "beamline/CLS/CLSSR570.h"

class CLSSR570Coordinator : public QObject
{
    Q_OBJECT
public:
	CLSSR570Coordinator(QString name, QString sr570PVBaseName, QObject *parent = 0);
	~CLSSR570Coordinator() {}

protected slots:
	void onCLSSR570ControlsConnectedChanged(bool);

	void onCLSSR570ConnectedChanged(bool);
	void onCLSSR570ValueChanged();

	void onSensNumValueChanged(double);
	void onSensNumDISAValueChanged(double);
	void onSensNumPROCValueChanged(double);
	void onSensUnitValueChanged(double);
	void onSensUnitDISAValueChanged(double);
	void onSensUnitPROCValueChanged(double);

private:
	enum PVUpdateState {
		Idle = 0xFF,
		SensPutValueChanged = 0x00,
		DisableSensNumDISA = 0x01,
		UpdateSensNum = 0x02,
		EnableSensNumDISA = 0x04,
		EnableSensNumPROC = 0x08,
		DisableSensUnitDISA = 0x10,
		UpdateSensUnit = 0x20,
		EnableSensUnitDISA = 0x40,
		EnableSensUnitPROC = 0x80
	};

	bool sensPutConnected_;
	bool controlSetConnected_;
	QString pvBaseName_;

	CLSSR570 * clsSR570_;
	AMControlSet *sr570CoordinatorControlSet_;

	int pvUpdateState_;
	double sensNumDISAValue_;
	double sensUnitDISAValue_;

	/* transform the PV value update from one state to another to make sure the PVs are updated accordingly */
	void pvUpdateStateTransition(int from, int to);
};

#endif // CLSSR570COORDINATOR_H
