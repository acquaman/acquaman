#ifndef AMBEAMLINESCANACTION_H
#define AMBEAMLINESCANACTION_H

#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QTime>
#include "AMBeamlineActionItem.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"

class AMBeamlineScanAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineScanAction(AMScanConfiguration *cfg, QString scanType = "", QString message = "", QObject *parent = 0);

	AMScanConfiguration* cfg() const { return cfg_;}
	virtual QString type() const;

signals:
	void progress(double, double);

public slots:
	virtual void start();

protected slots:
	virtual void scanCancelled();

protected:
	QString scanType_;
	AMScanConfiguration *cfg_;
	AMScanController * ctrl_;

private:
	QString type_;
};

class AMBeamlineScanActionView : public QWidget
{
Q_OBJECT
public:
	AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index = 0, QWidget *parent = 0);

	int index() const { return index_;}

public slots:
	void setIndex(int index);

protected slots:
	void updateScanNameLabel();
	void updateProgressBar(double elapsed, double total);
	void onScanFinished();

protected:
	AMBeamlineScanAction *scanAction_;
	int index_;

	QLabel *scanNameLabel_;
	QProgressBar *progressBar_;
	QLabel *timeRemainingLabel_;
	QHBoxLayout *hl_;
};

#endif // AMBEAMLINESCANACTION_H
