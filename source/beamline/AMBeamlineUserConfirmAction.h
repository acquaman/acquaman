#ifndef AMBEAMLINEUSERCONFIRMACTION_H
#define AMBEAMLINEUSERCONFIRMACTION_H

#include "AMBeamlineActionItem.h"

#include <QToolButton>

// Wait for a user to confirm that they have achieved an action (no viable control feedback)

class AMBeamlineUserConfirmAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineUserConfirmAction(QObject *parent = 0);

	virtual AMBeamlineActionView* createView(int index);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void cleanup();

	virtual void userProceed();
	virtual void userCancel();
};

class AMBeamlineUserConfirmDetailedActionView : public AMBeamlineActionView
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

protected:
	AMBeamlineUserConfirmAction *userConfirmAction_;
	QLabel *messageLabel_;
	QPushButton *proceedButton_;
	QPushButton *cancelButton_;
	QToolButton *finishedState_;
	QHBoxLayout *mainHL_;
	QVBoxLayout *buttonsVL_;
};

#endif // AMBEAMLINEUSERCONFIRMACTION_H
