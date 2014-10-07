#ifndef CLSSR570COORDINATOR_H
#define CLSSR570COORDINATOR_H

#include <QObject>

#include <beamline/AMPVControl.h>

class CLSSR570Coordinator : public QObject
{
    Q_OBJECT
public:
	explicit CLSSR570Coordinator(QString name, QString pvBaseName, QObject *parent = 0);
	~CLSSR570Coordinator();

protected slots:
	void onSensitivityControlConnectedChanged(bool);
	void onSensitivityControlValueChanged(double);
	void onSensPutControlConnectedChanged(bool);
	void onSensPutControlValueChanged(double);
	void onSensNumControlConnectedChanged(bool);
	void onSensUnitControlConnectedChanged(bool);

protected:
	QString pvBaseName_;
	bool sensPutConnected_;
	bool sensNumConnected_;
	bool sensUnitConnected_;

	AMControl sensitivityPVControl_;
	AMControl *sensPutPVControl_;
	AMSinglePVControl *sensNumPVControl_;
	AMSinglePVControl *sensUnitPVControl_;

};

#endif // CLSSR570COORDINATOR_H
