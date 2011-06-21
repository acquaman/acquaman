#ifndef AMBEAMLINEHIGHVOLTAGECHANNELTOGGLEACTION_H
#define AMBEAMLINEHIGHVOLTAGECHANNELTOGGLEACTION_H

#include <QProgressBar>
#include <QTime>
#include <QTimer>

#include "AMBeamlineActionItem.h"
#include "AMHighVoltageChannel.h"

class AMBeamlineHighVoltageChannelToggleAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	explicit AMBeamlineHighVoltageChannelToggleAction(AMHighVoltageChannel *highVoltageChannel, QObject *parent = 0);

	virtual AMBeamlineActionItemView* createView(int index = 0);

	virtual AMBeamlineActionItem* createCopy() const;

	virtual AMHighVoltageChannel* highVoltageChannel();
	virtual AMHighVoltageChannel::highVoltageChannelPowerState setpoint();

signals:
	void progress(double, double);

public slots:
	virtual void start();
	virtual void cancel();
	virtual void setHighVoltageChannel(AMHighVoltageChannel *highVoltageChannel);
	virtual void setSetpoint(AMHighVoltageChannel::highVoltageChannelPowerState setpoint);
	virtual void cleanup(){}

protected slots:
	void delayedStart(bool ready);
	virtual void onConnected(bool connected);
	virtual void onVoltageChanged(double voltage);
	virtual bool checkReady();
	virtual void onStarted();
	virtual void onSucceeded();
	virtual void onFailed(int explanation);
	virtual void onFinsihed();
	virtual void calculateProgress();

protected:
	AMHighVoltageChannel *highVoltageChannel_;
	AMHighVoltageChannel::highVoltageChannelPowerState setpoint_;
	QTimer progressTimer_;
};

#endif // AMBEAMLINEHIGHVOLTAGECHANNELTOGGLEACTION_H
