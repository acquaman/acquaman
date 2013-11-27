#ifndef CLSAMPTEKSDD123DETAILEDDETECTORVIEW_H
#define CLSAMPTEKSDD123DETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

class CLSAmptekSDD123DetectorNew;
class CLSAmptekDetectorROIView;
class AMControlEditor;
class CLSAmptekDetectorConfigurationView;

class CLSAmptekDetailedDetectorView : public AMXRFDetailedDetectorView
{
Q_OBJECT
public:
	CLSAmptekDetailedDetectorView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent = 0);

	virtual void buildDetectorView();

protected slots:
	void onEditAmptekROIsButtonClicked();
	void onEditAmptekConfigurationButtonClicked();

protected:
	CLSAmptekSDD123DetectorNew *amptekDetector_;

	QPushButton *editAmptekROIsButton_;
	QPushButton *editAmptekConfigurationButton_;
	QDoubleSpinBox *eVPerBinDoubleSpinBox_;

	AMControlEditor *fastCountsControlEditor_;
	AMControlEditor *averageFastCountsControlEditor_;
	AMControlEditor *slowCountsControlEditor_;
	AMControlEditor *averageSlowCountsControlEditor_;
	AMControlEditor *accumulationTimeControlEditor_;
	AMControlEditor *averageAccumulationTimeControlEditor_;
	AMControlEditor *realTimeControlEditor_;
	AMControlEditor *averageRealTimeControlEditor_;
	AMControlEditor *liveTimeControlEditor_;
	AMControlEditor *averageLiveTimeControlEditor_;

	CLSAmptekDetectorROIView *roiView_;
	CLSAmptekDetectorConfigurationView *configurationView_;
};

class CLSAmptekDetectorROIView : public QWidget
{
Q_OBJECT
public:
	CLSAmptekDetectorROIView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent = 0);

protected slots:
	void onDetectorConnected(bool isConnected);

	void onLowIndexValueChanged(int index);
	void onHighIndexValueChanged(int index);

	void onLowIndexLineEditEditingFinished(int index);
	void onHighIndexLineEditEditingFinished(int index);

protected:
	void roiEditingFinishedHelper(int index);

protected:
	CLSAmptekSDD123DetectorNew *detector_;

	QList<QLineEdit*> roiLowIndexLineEdits_;
	QList<QLineEdit*> roiHighIndexLineEdits_;

	QSignalMapper *roiLowIndexLineEditsMapper_;
	QSignalMapper *roiHighIndexLineEditsMapper_;
};

class CLSAmptekDetectorConfigurationView : public QWidget
{
Q_OBJECT
public:
	CLSAmptekDetectorConfigurationView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent = 0);

protected:
	CLSAmptekSDD123DetectorNew *detector_;

	/// The total gain for the detector
	AMControlEditor *totalGainControlEditor_;
	/// The number of channels control
	AMControlEditor *mcaChannelsControlEditor_;
	/// The pile up rejection is either on or off
	AMControlEditor *pileUpRejectionControlEditor_;
	/// The temperature for the cooler
	AMControlEditor *thermoelectricCoolerControlEditor_;
	/// The fast threshold for the detector
	AMControlEditor *fastThresholdControlEditor_;
	/// The slow threshold for the detector
	AMControlEditor *slowThresholdControlEditor_;
	/// The peaking time for the detector
	AMControlEditor *peakingTimeControlEditor_;
	/// The fast channel peaking time for the detector
	AMControlEditor *fastChannelPeakingTimeControlEditor_;
};

#endif // CLSAMPTEKSDD123DETAILEDDETECTORVIEW_H
