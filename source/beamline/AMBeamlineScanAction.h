#ifndef AMBEAMLINESCANACTION_H
#define AMBEAMLINESCANACTION_H

#include "AMBeamlineActionItem.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"

class AMBeamlineScanAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineScanAction(AMScanConfiguration *cfg, QString type = "", QString message = "", QObject *parent = 0);

signals:

public slots:
	virtual void start();

protected slots:
	virtual void scanCancelled();

protected:
	QString type_;
	AMScanConfiguration *cfg_;
	AMScanController * ctrl_;
};

#endif // AMBEAMLINESCANACTION_H
