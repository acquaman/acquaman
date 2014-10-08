#ifndef CLSSR570COORDINATOR_H
#define CLSSR570COORDINATOR_H

#include <QObject>

#include <beamline/AMPVControl.h>
#include <beamline/CLS/CLSSR570.h>

class CLSSR570Coordinator : public QObject
{
    Q_OBJECT
public:
	CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent = 0);
	~CLSSR570Coordinator();

protected slots:
	void onCLSSR570ConnectedChanged(bool);
	void onCLSSR570ValueChanged();
	void onSensNumControlConnectedChanged(bool);
	void onSensNumDISAControlConnectedChanged(bool);
	void onSensNumDISVControlConnectedChanged(bool);
	void onSensUnitControlConnectedChanged(bool);
	void onSensUnitDISAControlConnectedChanged(bool);
	void onSensUnitDISVControlConnectedChanged(bool);

protected:
	QString pvBaseName_;
	CLSSR570 * clsSR570_;

	bool sensPutConnected_;
	bool sensNumConnected_;
	bool sensNumDISAConnected_;
	bool sensNumDISVConnected_;
	bool sensUnitConnected_;
	bool sensUnitDISAConnected_;
	bool sensUnitDISVConnected_;

	double sensNumDISAValue_;
	double sensNumDISVValue_;
	double sensUnitDISAValue_;
	double sensUnitDISVValue_;

	AMSinglePVControl *sensNumPVControl_;
	AMSinglePVControl *sensNumDISAPVControl_;
	AMSinglePVControl *sensNumDISVPVControl_;
	AMSinglePVControl *sensUnitPVControl_;
	AMSinglePVControl *sensUnitDISAPVControl_;
	AMSinglePVControl *sensUnitDISVPVControl_;
};

#endif // CLSSR570COORDINATOR_H
