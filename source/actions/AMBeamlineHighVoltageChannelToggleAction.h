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


#ifndef AMBEAMLINEHIGHVOLTAGECHANNELTOGGLEACTION_H
#define AMBEAMLINEHIGHVOLTAGECHANNELTOGGLEACTION_H

#include <QProgressBar>
#include <QTime>
#include <QTimer>

#include "AMBeamlineActionItem.h"
#include "beamline/AMHighVoltageChannel.h"

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
