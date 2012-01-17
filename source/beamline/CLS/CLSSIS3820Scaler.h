#ifndef CLSSIS3820SCALER_H
#define CLSSIS3820SCALER_H

#include "beamline/AMControlSet.h"

class CLSSIS3820ScalerChannel;
class AMBeamlineActionItem;

class CLSSIS3820Scaler : public QObject
{
Q_OBJECT
public:
	CLSSIS3820Scaler(const QString &baseName, QObject *parent = 0);

	bool isConnected() const;

	bool isScanning() const;
	bool isContinuous() const;
	double dwellTime() const;
	int scansPerBuffer() const;
	int totalScans() const;
	QVector<int> reading() const;

	CLSSIS3820ScalerChannel* channelAt(int index);
	AMOrderedList<CLSSIS3820ScalerChannel*> channels();

	AMControl* startToggleControl();
	AMControl* continuousToggleControl();
	AMControl* dwellTimeControl();
	AMControl* scansPerBufferControl();
	AMControl* totalScansControl();

	AMBeamlineActionItem* createStartAction(bool setScanning);
	AMBeamlineActionItem* createContinuousEnableAction(bool enableContinuoius);
	AMBeamlineActionItem* createDwellTimeAction(double dwellTime);
	AMBeamlineActionItem* createScansPerBufferAction(int scansPerBuffer);
	AMBeamlineActionItem* createTotalScansAction(int totalScans);

public slots:
	void setScanning(bool isScanning);
	void setContinuous(bool isContinuous);
	void setDwellTime(double dwellTime);
	void setScansPerBuffer(int scansPerBuffer);
	void setTotalScans(int totalScans);

signals:
	void scanningChanged(bool isScanning);
	void continuousChanged(bool isContinuous);
	void dwellTimeChanged(double dwellTime);
	void scansPerBufferChanged(int scansPerBuffer);
	void totalScansChanged(int totalScans);
	void readingChanged(QVector<int> reading);
	void connectedChanged(bool isConnected);

protected slots:
	void onScanningToggleChanged(double setScanning);
	void onContinuousToggleChanged(double setContinuous);
	void onScanPerBufferChanged(double scansPerBuffer);
	void onTotalScansChanged(double totalScans);
	void onReadingChanged();
	void onConnectedChanged(bool connected);

protected:
	AMOrderedList<CLSSIS3820ScalerChannel*> scalerChannels_;

	AMControl *startToggle_;
	AMControl *continuousToggle_;
	AMControl *dwellTime_;
	AMControl *scanPerBuffer_;
	AMControl *totalScans_;
	AMControl *reading_;

	AMControlSet *allControls_;

	bool connectedOnce_;
};

class CLSSIS3820ScalerChannel : public QObject
{
Q_OBJECT
public:
	CLSSIS3820ScalerChannel(const QString &baseName, int index, QObject *parent = 0);

	bool isConnected() const;
	bool isEnabled() const;
	int reading() const;

	AMBeamlineActionItem* createEnableAction(bool setEnabled);

public slots:
	void setEnabled(bool isEnabled);

signals:
	void enabledChanged(bool isEnabled);
	void readingChanged(int reading);
	void readingChanged(const QString &reading);
	void connected(bool isConnected);

protected slots:
	void onChannelEnabledChanged(double enabled);
	void onChannelReadingChanged(double reading);

protected:
	AMControl *channelEnable_;
	AMControl *channelReading_;
	AMControlSet *allControls_;
};

#endif // CLSSIS3820SCALER_H
