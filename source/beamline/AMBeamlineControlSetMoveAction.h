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
	virtual AMControlSetInfo* setpoint();

signals:
	void progress(double, double);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setControlSet(AMControlSet *controlSet);
	virtual bool setSetpoint(AMControlSetInfo *setpoint); // It's copying from this controlSetInfo. So if you change your copy it won't do anything
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
	AMControlSetInfo *setpoint_;
	AMControlSetInfo *startPoint_;
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
