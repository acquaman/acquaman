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


#ifndef CLSCAEN2527CHANNELBASICVIEW_H
#define CLSCAEN2527CHANNELBASICVIEW_H

#include <QGroupBox>

#include "beamline/AMHighVoltageChannel.h"
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class AMControlEditor;
class CLSCAEN2527HVChannel;

class CLSCAEN2527ChannelBasicView : public QGroupBox
{
Q_OBJECT
public:
	CLSCAEN2527ChannelBasicView(CLSCAEN2527HVChannel *hvChannel, QWidget *parent = 0);

protected slots:
	void onHVChannelConnected(bool connect);
	void onPowerStateChanged(AMHighVoltageChannel::highVoltageChannelPowerState powerState);
	void onToggleButtonToggled(bool toggled);

protected:
	CLSCAEN2527HVChannel *hvChannel_;
	QLineEdit *descriptionEdit_;
	AMControlEditor *demandCE_;
	AMControlEditor *voltageCE_;
	QPushButton *toggleButton_;
	AMControlEditor *statusCE_;
	AMControlEditor *currentCE_;
	QVBoxLayout *vl_;
};

#endif // CLSCAEN2527CHANNELBASICVIEW_H
