/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMBEAMLINECONTROLMOVEACTION_H
#define AMBEAMLINECONTROLMOVEACTION_H

#include <QProgressBar>
#include <QTime>
#include <QIcon>

#include "AMBeamlineActionItem.h"
#include "beamline/AMControl.h"

#define AMBEAMLINECONTROLMOVEACTION_ONSUCCEEDED_MESSAGE 270501
#define AMBEAMLINECONTROLMOVEACTION_ONFAILED_MESSAGE 270502
#define AMBEAMLINECONTROLMOVEACTION_ONFINISHED_MESSAGE 270503
#define AMBEAMLINECONTROLMOVEACTION_CALCULATEPROGRESS_MESSAGE 270504

class AMBeamlineControlMoveAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineControlMoveAction(AMControl *control, QObject *parent = 0);

	virtual AMBeamlineActionItemView* createView(int index = 0);

	virtual AMControl* control();
	virtual double setpoint();

signals:
	void progress(double, double);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setControl(AMControl *control);
	virtual bool setSetpoint(double setpoint);
	virtual void cleanup(){}

protected slots:
//	virtual void initialize();
	void delayedStart(bool ready);
	virtual void onMovingChanged(bool moving);
	virtual void onConnected(bool connected);
	virtual void checkReady();
	virtual void onStarted();
	virtual void onSucceeded();
	virtual void onFailed(int explanation);
	virtual void onFinished();
	virtual void calculateProgress();

protected:
	AMControl *control_;
	double setpoint_;
	double startPoint_;
	QTimer progressTimer_;
};

class AMBeamlineControlMoveActionView : public AMBeamlineActionItemView
{
	Q_OBJECT
public:
	AMBeamlineControlMoveActionView(AMBeamlineControlMoveAction *moveAction, int index = 0, QWidget *parent = 0);

public slots:
	void setIndex(int index);
	virtual void setAction(AMBeamlineActionItem *action);

signals:
	void actionStarted(AMBeamlineActionItem *action);
	void actionSucceeded(AMBeamlineActionItem *action);
	void actionFailed(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged();
	virtual void updateProgressBar(double elapsed, double total);
	virtual void onStopCancelButtonClicked();
	virtual void onPlayPauseButtonClicked();

	virtual void onStarted();
	virtual void onSucceeded();
	virtual void onFailed(int explanation);

protected:
	AMBeamlineControlMoveAction *moveAction_;

	QLabel *infoLabel_;
	QProgressBar *progressBar_;
	QLabel *timeRemainingLabel_;
	QPushButton *stopCancelButton_;
	QPushButton *playPauseButton_;
	QHBoxLayout *hl_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;
};

#endif // AMBEAMLINECONTROLMOVEACTION_H
