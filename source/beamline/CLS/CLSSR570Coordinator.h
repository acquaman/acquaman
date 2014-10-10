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
	~CLSSR570Coordinator();

protected slots:
	void onCLSSR570ConnectedChanged(bool);
	void onCLSSR570ValueChanged();

protected:

	AMControlSet* clsSR570ControlSet_[];

	QString pvBaseName_;
	CLSSR570 * clsSR570_;
	AMControlSet *sr570CoordinatorControlSet;

	bool sensPutConnected_;
};

#endif // CLSSR570COORDINATOR_H
