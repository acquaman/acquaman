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

	virtual AMBeamlineActionItemView* createView(int index = 0);

	virtual AMBeamlineActionItem* createCopy() const;

	AMScanConfiguration* cfg() const { return cfg_;}
	virtual bool isRunning() const;
	virtual bool isPaused() const;

signals:
	void progress(double, double);
	void descriptionChanged();

public slots:
	virtual void start();
	virtual void pause(bool pause);
	virtual void cancel();

	/// move this scan action's partially completed scan to the user's recycle bin. Only call after calling cancel().
	/// \todo REALLY IMPORTANT!
	void discardScan() {}

	virtual void cleanup();

protected slots:
	virtual void initialize();
	void delayedStart(bool ready);
	virtual void onScanStarted();
	virtual void onScanCancelled();
	virtual void onScanSucceeded();
	virtual void onScanFailed();
	virtual void onBeamlineScanningChanged(bool isScanning);
	virtual void onConfigurationChanged();

protected:
	AMScanConfiguration *cfg_;
	AMScanController *ctrl_;
	bool keepOnCancel_;
};

class AMBeamlineScanActionView : public AMBeamlineActionItemView
{
Q_OBJECT
public:
	AMBeamlineScanActionView(AMBeamlineScanAction *scanAction, int index = 0, QWidget *parent = 0);

	AMBeamlineActionItem* action() { return scanAction_;}

public slots:
	void setIndex(int index);
	void setAction(AMBeamlineActionItem *action);

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
	void mouseDoubleClickEvent(QMouseEvent *);

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

	AMScanConfigurationView *configurationView_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;
};

#endif // AMBEAMLINESCANACTION_H
