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


#ifndef AMBEAMLINEUSERCONFIRMACTION_H
#define AMBEAMLINEUSERCONFIRMACTION_H

#include "AMBeamlineActionItem.h"

class QToolButton;
class QVBoxLayout;

// Wait for a user to confirm that they have achieved an action (no viable control feedback)

class AMBeamlineUserConfirmAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineUserConfirmAction(QObject *parent = 0);

	virtual AMBeamlineActionItemView* createView(int index = 0);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void cleanup();

	virtual void userProceed();
	virtual void userCancel();
};

class AMBeamlineUserConfirmDetailedActionView : public AMBeamlineActionItemView
{
Q_OBJECT
public:
	AMBeamlineUserConfirmDetailedActionView(AMBeamlineUserConfirmAction *userConfirmAction, int index = 0, QWidget *parent = 0);

public slots:
	void setIndex(int index);
	virtual void setAction(AMBeamlineActionItem *action);

signals:
	void actionStarted(AMBeamlineActionItem *action);
	void actionSucceeded(AMBeamlineActionItem *action);
	void actionFailed(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged();
	virtual void onStopCancelButtonClicked();
	virtual void onPlayPauseButtonClicked();

	void onActionStarted();
	void onActionSucceeded();
	void onActionFailed(int explanation);

	void onHelpButtonClicked();

protected:
	AMBeamlineUserConfirmAction *userConfirmAction_;
	QLabel *messageLabel_;
	QPushButton *proceedButton_;
	QPushButton *cancelButton_;
	QToolButton *finishedState_;
	QToolButton *helpButton_;
	QHBoxLayout *mainHL_;
	QVBoxLayout *buttonsVL_;
};

#endif // AMBEAMLINEUSERCONFIRMACTION_H
