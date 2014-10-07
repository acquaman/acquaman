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
	void onSensUnitControlConnectedChanged(bool);

protected:
	QString pvBaseName_;
	CLSSR570 * clsSR570_;

	bool sensPutConnected_;
	bool sensNumConnected_;
	bool sensUnitConnected_;

	AMSinglePVControl *sensNumPVControl_;
	AMSinglePVControl *sensUnitPVControl_;
};

#endif // CLSSR570COORDINATOR_H
