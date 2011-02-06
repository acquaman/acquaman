/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMBEAMLINESCANACTION_H
#define AMBEAMLINESCANACTION_H

#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QTime>
#include <QMessageBox>
#include "AMBeamlineActionItem.h"
#include "acquaman/AMScanConfiguration.h"

class AMBeamlineScanAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineScanAction(AMScanConfiguration *cfg, QObject *parent = 0);

	AMScanConfiguration* cfg() const { return cfg_;}
	virtual QString type() const;
	virtual bool isRunning() const;
	virtual bool isPaused() const;
	// unused and unimplemented: virtual bool isReinitialized() const;

signals:
	void progress(double, double);

public slots:
	virtual void start();
	virtual void pause(bool pause);
	virtual void cancel();
	virtual void cancelButKeep();
	virtual void reset(bool delayInitialized = false);
	virtual void cleanup();

protected slots:
	virtual void initialize();
	void delayedStart(bool ready);
	virtual void onScanStarted();
	virtual void onScanCancelled();
	virtual void onScanSucceeded();
	virtual void onBeamlineScanningChanged(bool isScanning);

protected:
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
	QHBoxLayout *hl_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;

private:
	QString viewType_;
};

#endif // AMBEAMLINESCANACTION_H
