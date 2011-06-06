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
