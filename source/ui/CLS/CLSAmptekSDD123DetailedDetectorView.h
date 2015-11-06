/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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
 	virtual ~CLSAmptekDetailedDetectorView();
	CLSAmptekDetailedDetectorView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent = 0);

	virtual void buildDetectorView();

protected slots:
	void onEditAmptekROIsButtonClicked();
	void onEditAmptekConfigurationButtonClicked();

	void onDetectorTemperatureChanged(double temperature);

	void onContinuousTestButtonClicked();
	void onGetLastContinuousTestButtonClicked();

protected:
	CLSAmptekSDD123DetectorNew *amptekDetector_;

	QPushButton *editAmptekROIsButton_;
	QPushButton *editAmptekConfigurationButton_;
	QDoubleSpinBox *eVPerBinDoubleSpinBox_;
	QLabel *temperatureLabel_;

	AMControlEditor *fastCountsControlEditor_;
	AMControlEditor *slowCountsControlEditor_;
	AMControlEditor *accumulationTimeControlEditor_;
	AMControlEditor *realTimeControlEditor_;
	AMControlEditor *liveTimeControlEditor_;

	CLSAmptekDetectorROIView *roiView_;
	CLSAmptekDetectorConfigurationView *configurationView_;

	QPushButton *continuousTestButton_;
	QPushButton *getLastContinuousTestButton_;
};

class CLSAmptekDetectorROIView : public QWidget
{
Q_OBJECT
public:
 	virtual ~CLSAmptekDetectorROIView();
	CLSAmptekDetectorROIView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent = 0);

protected slots:
	void onDetectorConnected(bool isConnected);

	void onLowIndexValueChanged(int index);
	void onHighIndexValueChanged(int index);

	void onLowIndexLineEditEditingFinished(int index);
	void onHighIndexLineEditEditingFinished(int index);

	void onLowIndexDoubleSpinBoxEditingFinished(int index);
	void onHighIndexDoubleSpinBoxEditingFinished(int index);

	/// Slot for handling changes to the detectors evPerBit
	void onDetectorEvPerBinChanged(double newValue);

protected:
	void roiLineEditingFinishedHelper(int index);
	void roiSpinBoxEditingFinishedHelper(int index);

protected:
	CLSAmptekSDD123DetectorNew *detector_;

	QList<QLineEdit*> roiLowIndexLineEdits_;
	QList<QLineEdit*> roiHighIndexLineEdits_;
	QList<QDoubleSpinBox*> roiLowIndexDoubleSpinBoxes_;
	QList<QDoubleSpinBox*> roiHighIndexDoubleSpinBoxes_;
	/// Widget for displaying the eV/bin value of the detector (declared here to allow it to be updated)
	QLabel* roiEditModeConversionRateLabel;

	QSignalMapper *roiLowIndexLineEditsMapper_;
	QSignalMapper *roiHighIndexLineEditsMapper_;

	QSignalMapper* roiLowIndexDoubleSpinBoxesMapper_;
	QSignalMapper* roiHighIndexDoubleSpinBoxesMapper_;
};

class CLSAmptekDetectorConfigurationView : public QWidget
{
Q_OBJECT
public:
 	virtual ~CLSAmptekDetectorConfigurationView();
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
