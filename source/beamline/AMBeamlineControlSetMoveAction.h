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


#ifndef AMBEAMLINECONTROLSETMOVEACTION_H
#define AMBEAMLINECONTROLSETMOVEACTION_H

#include <QTimer>
#include <QProgressBar>

#include "AMBeamlineActionItem.h"
#include "AMControlSet.h"

class AMBeamlineControlSetMoveAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineControlSetMoveAction(AMControlSet *controlSet, QObject *parent = 0);

	virtual QString type() const;
	virtual AMControlSet* controlSet();
	virtual AMControlInfoList* setpoint();

signals:
	void progress(double, double);

public slots:
	virtual void start();
	virtual void cancel();
	/* NTBA March 14, 2011 David Chevrier
	   Look into passing const references
	   */
	virtual void setControlSet(AMControlSet *controlSet);
	// It's copying from this controlSetInfo. So if you change your copy it won't do anything
	virtual bool setSetpoint(AMControlInfoList *setpoint);
	virtual void cleanup(){}

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
	AMControlInfoList *setpoint_;
	AMControlInfoList *startPoint_;
	QTimer progressTimer_;

private:
	QString type_;
};

class AMBeamlineControlSetMoveActionView : public AMBeamlineActionView
{
Q_OBJECT
public:
	AMBeamlineControlSetMoveActionView(AMBeamlineControlSetMoveAction *controlSetAction, int index = 0, QWidget *parent = 0);

	virtual QString viewType() const;

public slots:
	void setIndex(int index);
	void setAction(AMBeamlineControlSetMoveAction *controlSetAction);

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
	AMBeamlineControlSetMoveAction *controlSetAction_;
	QLabel *infoLabel_;
	QProgressBar *progressBar_;
	QLabel *timeRemainingLabel_;
	QPushButton *stopCancelButton_;
	QPushButton *playPauseButton_;
	QHBoxLayout *hl_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;

private:
	QString viewType_;
};

#endif // AMBEAMLINECONTROLSETMOVEACTION_H
