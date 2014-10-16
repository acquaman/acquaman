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
	enum PVUpdateStatus {
		Idle = -1,
		SensPutValueChanged = 0,
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

	int pvUpdateStatus_;
	double sensNumDISAValue_;
	double sensUnitDISAValue_;

	void pvUpdateStatusTransition(int from, int to);
};

#endif // CLSSR570COORDINATOR_H
