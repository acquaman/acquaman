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
	virtual void scanSucceeded();

protected:
	QString scanType_;
	AMScanConfiguration *cfg_;
	AMScanController * ctrl_;

private:
	QString type_;
};

//class AMBeamlineScanActionView : public QWidget
class AMBeamlineScanActionView : public QFrame
{
Q_OBJECT
public:
	AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index = 0, QWidget *parent = 0);

	int index() const { return index_;}
	AMBeamlineActionItem* action() { return scanAction_;}

public slots:
	void setIndex(int index);
	void setAction(AMBeamlineScanAction *scanAction);
	void defocusItem();

signals:
	void focusRequested(AMBeamlineActionItem *action);
	void removeRequested(AMBeamlineActionItem *action);
	void scanSuceeded(AMBeamlineActionItem *action);

protected slots:
	void updateScanNameLabel();
	void updateProgressBar(double elapsed, double total);
	void onScanStarted();
	void onScanFinished();
	void onRemoveButtonClicked();

protected:
	void mousePressEvent(QMouseEvent *event);

	void updateLook();

protected:
	AMBeamlineScanAction *scanAction_;
	int index_;
	bool inFocus_;

	QLabel *scanNameLabel_;
	QProgressBar *progressBar_;
	QLabel *timeRemainingLabel_;
	QPushButton *removeButton_;
	QHBoxLayout *hl_;
};

#endif // AMBEAMLINESCANACTION_H
