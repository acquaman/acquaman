#ifndef AMBEAMLINESCANACTION_H
#define AMBEAMLINESCANACTION_H

#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include "AMBeamlineActionItem.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"

class AMBeamlineScanAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineScanAction(AMScanConfiguration *cfg, QString type = "", QString message = "", QObject *parent = 0);

	AMScanConfiguration* cfg() const { return cfg_;}

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

class AMBeamlineScanActionView : public QWidget
{
Q_OBJECT
public:
	AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, QWidget *parent = 0);

protected:
	QLabel *scanNameLabel_;
	QProgressBar *progressBar_;
	QHBoxLayout *hl_;
};

#endif // AMBEAMLINESCANACTION_H
