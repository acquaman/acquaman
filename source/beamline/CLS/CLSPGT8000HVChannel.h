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


#ifndef CLSPGT8000HVCHANNEL_H
#define CLSPGT8000HVCHANNEL_H

#include "beamline/AMHighVoltageChannel.h"

class AMControl;
class AMProcessVariable;

class CLSPGT8000HVChannel : public AMHighVoltageChannel
{
Q_OBJECT
public:
	CLSPGT8000HVChannel(const QString &name, const QString &pvBaseName, QObject *parent = 0);

	virtual QString description() const;

	virtual bool isConnected() const;

	virtual double demand() const;
	virtual double voltage() const;

	virtual bool isFullyPowered() const;

	virtual QString status() const;
	virtual double current() const;

public slots:
	virtual void setDescription(const QString &description);
	virtual void setDemand(double demand);
	virtual void setPowerState(highVoltageChannelPowerState powerState);

protected slots:
	void onChildConnected(bool connect);
	void onDemandChanged(double demand);
	void onVoltageChanged(double voltage);
	void onToggleChanged(double toggle);

protected:
	bool wasConnected_;
	bool poweringDown_;
	QString description_;
	AMControl *demand_;
	AMControl *voltage_;
	AMControl *toggle_;
};

#endif // CLSPGT8000HVCHANNEL_H
