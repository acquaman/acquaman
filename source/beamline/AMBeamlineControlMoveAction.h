#ifndef AMBEAMLINECONTROLMOVEACTION_H
#define AMBEAMLINECONTROLMOVEACTION_H

#include <QProgressBar>

#include "AMBeamlineActionItem.h"
#include "AMControl.h"

class AMBeamlineControlMoveAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineControlMoveAction(AMControl *control, QString message = "", QObject *parent = 0);

	virtual QString type() const;
	virtual AMControl* control();
	virtual double setpoint();

signals:

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setControl(AMControl *control);
	virtual bool setSetpoint(double setpoint);

protected slots:
	virtual void onStarted();
	virtual void onSucceeded();
	virtual void onFailed(int explanation);

protected:
	AMControl *control_;
	double setpoint_;
	bool ready_;

private:
	QString type_;
};

class AMBeamlineControlMoveActionView : public AMBeamlineActionView
{
	Q_OBJECT
public:
	AMBeamlineControlMoveActionView(AMBeamlineControlMoveAction *moveAction, int index = 0, QWidget *parent = 0);

	virtual QString viewType() const;

public slots:
	void setIndex(int index);
	void setAction(AMBeamlineControlMoveAction *moveAction);

signals:
	void actionSucceeded(AMBeamlineActionItem *action);
	void actionStarted(AMBeamlineActionItem *action);

protected slots:
	virtual void onInfoChanged();
	virtual void onStopCancelButtonClicked();
	virtual void onPlayPauseButtonClicked();
	virtual void onHideButtonClicked();

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
	QPushButton *hideButton_;
	QHBoxLayout *hl_;

	QIcon closeIcon_, stopIcon_, startIcon_, pauseIcon_;

private:
	QString viewType_;
};

#endif // AMBEAMLINECONTROLMOVEACTION_H
