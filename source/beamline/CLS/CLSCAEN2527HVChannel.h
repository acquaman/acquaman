#ifndef CLSCAEN2527HVCHANNEL_H
#define CLSCAEN2527HVCHANNEL_H

#include "beamline/AMHighVoltageChannel.h"

class AMControl;
class AMProcessVariable;

class CLSCAEN2527HVChannel : public AMHighVoltageChannel
{
Q_OBJECT
public:
	CLSCAEN2527HVChannel(const QString &name, const QString &pvBaseName, AMHighVoltageChannel::highVoltageChannelPolarity polarity, QObject *parent = 0);

	virtual QString description() const;

	virtual bool isConnected() const;

	virtual double demand() const;
	virtual double voltage() const;

	virtual QString status() const;

	virtual double current() const;

	AMProcessVariable* descriptionControl();
	AMProcessVariable* descriptionFbkControl();
	AMControl* demandControl();
	AMControl* voltageControl();
	AMControl* toggleControl();
	AMControl* statusControl();
	AMControl* currentControl();

public slots:
	virtual void setDescription(const QString &description);
	virtual void setDemand(double demand);
	virtual void setPowerState(AMHighVoltageChannel::highVoltageChannelPowerState powerState);
	virtual bool setPolarity(AMHighVoltageChannel::highVoltageChannelPolarity polarity);

protected slots:
	void onDescriptionFbkChanged(const QString &description);
	void onChildConnected(bool connect);
	void onDemandChanged(double demand);
	void onVoltageChanged(double voltage);
	void onToggleChanged(double toggle);
	void onStatusChanged(double status);
	void onCurrentChanged(double current);

protected:
	bool wasConnected_;
	bool poweringDown_;
	AMProcessVariable *description_;
	AMProcessVariable *descriptionFbk_;
	AMControl *demand_;
	AMControl *voltage_;
	AMControl *toggle_;
	AMControl *status_;
	AMControl *current_;
};

#endif // CLSCAEN2527HVCHANNEL_H
