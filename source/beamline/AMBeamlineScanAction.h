#ifndef AMBEAMLINESCANACTION_H
#define AMBEAMLINESCANACTION_H

#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QTime>
#include <QMessageBox>
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
	bool isPaused() const;

signals:
	void progress(double, double);

public slots:
	virtual void start();
	virtual void pause(bool pause);
	virtual void cancel();
	virtual void cancelButKeep();

protected slots:
	virtual void scanCancelled();
	virtual void scanSucceeded();

protected:
	QString scanType_;
	AMScanConfiguration *cfg_;
	AMScanController * ctrl_;
	bool keepOnCancel_;

private:
	QString type_;
};

class AMBeamlineScanActionView : public AMBeamlineActionView
{
Q_OBJECT
public:
	AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index = 0, QWidget *parent = 0);

	int index() const { return index_;}
	AMBeamlineActionItem* action() { return scanAction_;}

	virtual QString viewType() const;

public slots:
	void setIndex(int index);
	void setAction(AMBeamlineScanAction *scanAction);

signals:
	void scanStarted(AMBeamlineActionItem *action);
	void scanSuceeded(AMBeamlineActionItem *action);
	void scanCancelled(AMBeamlineActionItem *action);

protected slots:
	void onInfoChanged();
	void updateScanNameLabel();
	void updateProgressBar(double elapsed, double total);
	void onScanStarted();
	void onScanFinished();
	void onScanFailed(int explanation);
	void onStopCancelButtonClicked();
	void onPlayPauseButtonClicked();
	void onHideButtonClicked();

protected:
	void updateLook();

protected:
	AMBeamlineScanAction *scanAction_;
	bool cancelLatch_;

	QLabel *scanNameLabel_;
	QProgressBar *progressBar_;
	QLabel *timeRemainingLabel_;
	QPushButton *stopCancelButton_;
	QPushButton *playPauseButton_;
	QPushButton *hideButton_;
	QHBoxLayout *hl_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;

private:
	QString viewType_;
};

#endif // AMBEAMLINESCANACTION_H
