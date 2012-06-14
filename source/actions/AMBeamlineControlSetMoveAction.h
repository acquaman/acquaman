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


#ifndef AMBEAMLINECONTROLSETMOVEACTION_H
#define AMBEAMLINECONTROLSETMOVEACTION_H

#include <QTimer>
#include <QProgressBar>

#include "AMBeamlineActionItem.h"
#include "beamline/AMControlSet.h"

class QToolButton;

#define AMBEAMLINECONTROLSETMOVEACTION_ONFAILED_MESSAGE 270601

class AMBeamlineControlSetMoveAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineControlSetMoveAction(AMControlSet *controlSet, QObject *parent = 0);

	virtual AMBeamlineActionItemView* createView(int index = 0);

	virtual AMBeamlineActionItem* createCopy() const;

	virtual AMControlSet* controlSet();
	virtual AMControlInfoList setpoint();

signals:
	void progress(double elapsed, double total);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setControlSet(AMControlSet *controlSet);
	virtual bool setSetpoint(const AMControlInfoList &setpoint);
	virtual void cleanup();

protected slots:
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
	AMControlSet *controlSet_;
	AMControlInfoList setpoint_;
	AMControlInfoList fullSetpoint_; // Includes setpoint for all of the controls, even if the setpoint is a subset or a superset
	AMControlInfoList startpoint_;
	QTimer progressTimer_;
	int numSucceeded_;
};

class AMBeamlineControlSetMoveActionView : public AMBeamlineActionItemView
{
Q_OBJECT
public:
	AMBeamlineControlSetMoveActionView(AMBeamlineControlSetMoveAction *controlSetAction, int index = 0, QWidget *parent = 0);

public slots:
	void setIndex(int index);
	void setAction(AMBeamlineActionItem *action);

signals:
	void actionStarted(AMBeamlineActionItem *action);
	void actionSucceeded(AMBeamlineActionItem *action);
	void actionFailed(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged();
	virtual void updateProgressBar(double elapsed, double total);
	virtual void onStopCancelButtonClicked();
	virtual void onPlayPauseButtonClicked();
	void onPreviousNextChanged();
	void onMoveUpButtonClicked();
	void onMoveDownButtonClicked();

	virtual void onStarted();
	virtual void onSucceeded();
	virtual void onFailed(int explanation);

protected:
	AMBeamlineControlSetMoveAction *controlSetAction_;
	QLabel *infoLabel_;
	QProgressBar *progressBar_;
	QLabel *timeRemainingLabel_;
	QPushButton *stopCancelButton_;
	QPushButton *playPauseButton_;
	QToolButton *moveActionUpButton_;
	QToolButton *moveActionDownButton_;
	QHBoxLayout *hl_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;
};

#endif // AMBEAMLINECONTROLSETMOVEACTION_H
