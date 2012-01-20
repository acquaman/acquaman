/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

class QToolButton;

#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QTime>
#include <QMessageBox>
#include "AMBeamlineActionItem.h"
#include "acquaman/AMScanConfiguration.h"

#define AMBEAMLINEACTIONITEM_CANT_CREATE_CONTROLLER 27101
#define AMBEAMLINEACTIONITEM_SCAN_CANCELLED 27102
#define AMBEAMLINEACTIONITEM_CANT_SET_CURRENT_CONTROLLER 27103
#define AMBEAMLINEACTIONITEM_CANT_INITIALIZE_CONTROLLER 27104
#define AMBEAMLINEACTIONITEM_CANT_START_CONTROLLER 27105
#define AMBEAMLINEACTIONITEM_CANT_SAVE_TO_DB 27106
#define AMBEAMLINEACTIONITEM_CANT_CREATE_EXPORT_FOLDER 27107
#define AMBEAMLINEACTIONITEM_NO_REGISTERED_EXPORTER 27108
#define AMBEAMLINEACTIONITEM_NO_REGISTERED_EXPORTER_OPTION 27109
#define AMBEAMLINEACTIONITEM_NO_DATABASE_FOR_SCAN 27110

class AMScanExemplarDictionary;
#include "dataman/AMScanExemplar.h"

class AMBeamlineScanAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineScanAction(AMScanConfiguration *cfg, QObject *parent = 0);

	virtual AMBeamlineActionItemView* createView(int index = 0);

	virtual AMBeamlineActionItem* createCopy() const;

	AMScanConfiguration* cfg() const;
	virtual bool isRunning() const;
	virtual bool isPaused() const;

	QString lastSampleDescription() const;

	QString guessScanName() const;
	QString guessExportName() const;

	bool configurationLocked() const;

signals:
	void progress(double, double);
	void descriptionChanged();

	void exportMe(const QList<QUrl> &itemUrls);

public slots:
	virtual void start();
	virtual void pause(bool pause);
	virtual void cancel();

	/// move this scan action's partially completed scan to the user's recycle bin. Only call after calling cancel().
	/// \todo REALLY IMPORTANT!
	void discardScan() {}

	virtual void cleanup();

	void setLastSampleDescription(const QString &lastSampleDescription);

protected slots:
	virtual void initialize();
	void delayedStart(bool ready);
	virtual void onScanInitialized();
	virtual void onScanStarted();
	virtual void onScanCancelled();
	virtual void onScanSucceeded();
	virtual void onScanFailed();
	virtual void onBeamlineScanningChanged(bool isScanning);
	virtual void onConfigurationChanged();

	virtual void onConfigurationDestroyed();

protected:
	mutable AMScanConfiguration *cfg_;
	bool configurationLocked_;
	AMScanController *ctrl_;
	/// Used to retrieve the scan from the database if necessary
	int scanID_;
	bool keepOnCancel_;
	QString lastSampleDescription_;

	AMScanExemplar exemplar_;
	AMScanExemplarDictionary *nameDictionary_;
	AMScanExemplarDictionary *exportNameDictionary_;
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
	void onPreviousNextChanged();
	void onMoveUpButtonClicked();
	void onMoveDownButtonClicked();

	void onScanConfigurationDestroyed();

protected:
	void mouseDoubleClickEvent(QMouseEvent *);

protected:
	AMBeamlineScanAction *scanAction_;
	bool cancelLatch_;

	QLabel *scanNameLabel_;
	QLabel *exportNameLabel_;
	QProgressBar *progressBar_;
	QLabel *timeRemainingLabel_;
	QPushButton *stopCancelButton_;
	QPushButton *playPauseButton_;
	QToolButton *moveActionUpButton_;
	QToolButton *moveActionDownButton_;
	QHBoxLayout *hl_;
	QVBoxLayout *vl_;

	AMScanConfigurationView *configurationView_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;
};

#endif // AMBEAMLINESCANACTION_H
