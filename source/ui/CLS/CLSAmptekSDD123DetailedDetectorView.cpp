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


#include "CLSAmptekSDD123DetailedDetectorView.h"

#include <QLineEdit>
#include <QSignalMapper>
#include <QComboBox>
#include <QTabWidget>
#include <QToolTip>

#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"
#include "ui/beamline/AMControlEditor.h"

CLSAmptekDetailedDetectorView::CLSAmptekDetailedDetectorView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent) :
	AMXRFDetailedDetectorView(detector, parent)
{
	amptekDetector_ = detector;
}

CLSAmptekDetailedDetectorView::~CLSAmptekDetailedDetectorView(){}

void CLSAmptekDetailedDetectorView::buildDetectorView(){
	AMXRFDetailedDetectorView::buildDetectorView();

	editAmptekROIsButton_ = new QPushButton("Edit Amptek ROIs");
	rightLayout_->addWidget(editAmptekROIsButton_);

	editAmptekConfigurationButton_ = new QPushButton("Edit Amptek Configuration");
	rightLayout_->addWidget(editAmptekConfigurationButton_);

	QHBoxLayout *tempHBox;

	eVPerBinDoubleSpinBox_ = new QDoubleSpinBox();
	eVPerBinDoubleSpinBox_->setRange(0.5, 100);
	if(amptekDetector_->isConnected())
		eVPerBinDoubleSpinBox_->setValue(amptekDetector_->eVPerBin());
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("eV/bin"));
	tempHBox->addWidget(eVPerBinDoubleSpinBox_);
	rightLayout_->addLayout(tempHBox);
	connect(eVPerBinDoubleSpinBox_, SIGNAL(valueChanged(double)), amptekDetector_, SLOT(setEVPerBin(double)));

	temperatureLabel_ = new QLabel();
	if(amptekDetector_->isConnected())
		temperatureLabel_->setText(QString("%1").arg(amptekDetector_->detectorTemperature()));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Temperature"));
	tempHBox->addWidget(temperatureLabel_);
	connect(amptekDetector_, SIGNAL(detectorTemperatureChanged(double)), this, SLOT(onDetectorTemperatureChanged(double)));
	rightLayout_->addLayout(tempHBox);

	QLabel *tempLabel;

	tempLabel = new QLabel("Fast Counts (avg.)");
	fastCountsControlEditor_ = new AMControlEditor(amptekDetector_->fastCountsControl(this), amptekDetector_->averageFastCountsControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(fastCountsControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Slow Counts (avg.)");
	slowCountsControlEditor_ = new AMControlEditor(amptekDetector_->slowCountsControl(this), amptekDetector_->averageSlowCountsControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(slowCountsControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Accumulation (avg.)");
	accumulationTimeControlEditor_ = new AMControlEditor(amptekDetector_->accumulationTimeControl(this), amptekDetector_->averageAccumulationTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(accumulationTimeControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Real Time (avg.)");
	realTimeControlEditor_ = new AMControlEditor(amptekDetector_->realTimeControl(this), amptekDetector_->averageRealTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(realTimeControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Live Time (avg.)");
	liveTimeControlEditor_ = new AMControlEditor(amptekDetector_->liveTimeControl(this), amptekDetector_->averageLiveTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(liveTimeControlEditor_);
	rightLayout_->addLayout(tempHBox);

	roiView_ = new CLSAmptekDetectorROIView(amptekDetector_);
	connect(editAmptekROIsButton_, SIGNAL(clicked()), this, SLOT(onEditAmptekROIsButtonClicked()));

	configurationView_ = new CLSAmptekDetectorConfigurationView(amptekDetector_);
	connect(editAmptekConfigurationButton_, SIGNAL(clicked()), this, SLOT(onEditAmptekConfigurationButtonClicked()));

	spectraComboBox_->setCurrentIndex(0);
	plot_->axisScaleBottom()->setDataRangeConstraint(MPlotAxisRange(0, 2500));
}

void CLSAmptekDetailedDetectorView::onEditAmptekROIsButtonClicked(){
	roiView_->show();
	roiView_->raise();
}

void CLSAmptekDetailedDetectorView::onEditAmptekConfigurationButtonClicked(){
	configurationView_->show();
	configurationView_->raise();
}

void CLSAmptekDetailedDetectorView::onDetectorTemperatureChanged(double temperature){
	temperatureLabel_->setText(QString("%1").arg(temperature));
}

CLSAmptekDetectorROIView::CLSAmptekDetectorROIView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;
	QVBoxLayout *mainVL = new QVBoxLayout();
	QWidget* page1 = new QWidget();
	QWidget* page2 = new QWidget();
	QVBoxLayout* page1VL = new QVBoxLayout();
	QVBoxLayout* page2VL = new QVBoxLayout();

	roiLowIndexLineEditsMapper_ = new QSignalMapper();
	roiHighIndexLineEditsMapper_ = new QSignalMapper();
	roiLowIndexDoubleSpinBoxesMapper_ = new QSignalMapper();
	roiHighIndexDoubleSpinBoxesMapper_ = new QSignalMapper();


	//Set up header widgets (current detector eV->bin conversion)
	QTabWidget* editModeTab = new QTabWidget();

	roiEditModeConversionRateLabel = new QLabel();
	if(detector_->isConnected())
		roiEditModeConversionRateLabel->setText(QString("Note: Values will be set in increments of %1 eV/bin").arg(detector_->eVPerBin()));

	page1VL->addWidget(roiEditModeConversionRateLabel);

	//Set up widgets for each ROI
	QLabel *oneLineEditNameLabel;
	QLabel *oneDoubleSpinBoxNameLabel;
	QLineEdit *oneLowIndexLineEdit;
	QLineEdit *oneHighIndexLineEdit;
	QDoubleSpinBox *oneHighIndexDoubleSpinBox;
	QDoubleSpinBox *oneLowIndexDoubleSpinBox;
	QHBoxLayout *oneHBoxLayoutLineEdits;
	QHBoxLayout *oneHBoxLayoutDoubleSpinBoxes;

	for(int x = 0; x < 8; x++){
		oneLineEditNameLabel = new QLabel(QString("ROI %1").arg(x+1));
		oneDoubleSpinBoxNameLabel = new QLabel(QString("ROI %1").arg(x+1));
		oneLowIndexLineEdit = new QLineEdit();
		oneHighIndexLineEdit = new QLineEdit();
		oneHighIndexDoubleSpinBox = new QDoubleSpinBox();
		oneLowIndexDoubleSpinBox = new QDoubleSpinBox();


		if(detector_->isConnected())
		{
			oneLowIndexDoubleSpinBox->setMinimum(0);
			oneLowIndexDoubleSpinBox->setMaximum(1023 * detector->eVPerBin());
			oneLowIndexDoubleSpinBox->setSingleStep(detector_->eVPerBin());

			oneHighIndexDoubleSpinBox->setMinimum(0);
			oneHighIndexDoubleSpinBox->setMaximum(1023 * detector->eVPerBin());
			oneHighIndexDoubleSpinBox->setSingleStep(detector_->eVPerBin());

		}
		oneLowIndexDoubleSpinBox->setSuffix(QString(" eV"));
		oneHighIndexDoubleSpinBox->setSuffix(QString(" eV"));

		roiLowIndexLineEdits_.append(oneLowIndexLineEdit);
		roiHighIndexLineEdits_.append(oneHighIndexLineEdit);

		roiLowIndexDoubleSpinBoxes_.append(oneLowIndexDoubleSpinBox);
		roiHighIndexDoubleSpinBoxes_.append(oneHighIndexDoubleSpinBox);

		roiLowIndexLineEditsMapper_->setMapping(oneLowIndexLineEdit, x);
		roiHighIndexLineEditsMapper_->setMapping(oneHighIndexLineEdit, x);

		roiLowIndexDoubleSpinBoxesMapper_->setMapping(oneLowIndexDoubleSpinBox, x);
		roiHighIndexDoubleSpinBoxesMapper_->setMapping(oneHighIndexDoubleSpinBox, x);

		connect(oneLowIndexLineEdit, SIGNAL(editingFinished()), roiLowIndexLineEditsMapper_, SLOT(map()));
		connect(oneHighIndexLineEdit, SIGNAL(editingFinished()), roiHighIndexLineEditsMapper_, SLOT(map()));

		connect(oneLowIndexDoubleSpinBox, SIGNAL(editingFinished()), roiLowIndexDoubleSpinBoxesMapper_, SLOT(map()));
		connect(oneHighIndexDoubleSpinBox, SIGNAL(editingFinished()), roiHighIndexDoubleSpinBoxesMapper_, SLOT(map()));

		oneHBoxLayoutLineEdits = new QHBoxLayout();
		oneHBoxLayoutLineEdits->setSpacing(20);
		oneHBoxLayoutLineEdits->addWidget(oneLineEditNameLabel);
		oneHBoxLayoutLineEdits->addWidget(oneLowIndexLineEdit);
		oneHBoxLayoutLineEdits->addWidget(oneHighIndexLineEdit);

		oneHBoxLayoutDoubleSpinBoxes = new QHBoxLayout();
		oneHBoxLayoutDoubleSpinBoxes->setSpacing(20);
		oneHBoxLayoutDoubleSpinBoxes->addWidget(oneDoubleSpinBoxNameLabel);
		oneHBoxLayoutDoubleSpinBoxes->addWidget(oneLowIndexDoubleSpinBox);
		oneHBoxLayoutDoubleSpinBoxes->addWidget(oneHighIndexDoubleSpinBox);

		page1VL->addLayout(oneHBoxLayoutDoubleSpinBoxes);
		page2VL->addLayout(oneHBoxLayoutLineEdits);
	}
	connect(roiLowIndexLineEditsMapper_, SIGNAL(mapped(int)), this, SLOT(onLowIndexLineEditEditingFinished(int)));
	connect(roiHighIndexLineEditsMapper_, SIGNAL(mapped(int)), this, SLOT(onHighIndexLineEditEditingFinished(int)));

	connect(roiLowIndexDoubleSpinBoxesMapper_, SIGNAL(mapped(int)), this, SLOT(onLowIndexDoubleSpinBoxEditingFinished(int)));
	connect(roiHighIndexDoubleSpinBoxesMapper_, SIGNAL(mapped(int)), this, SLOT(onHighIndexDoubleSpinBoxEditingFinished(int)));

	connect(detector_, SIGNAL(connected(bool)), this, SLOT(onDetectorConnected(bool)));
	connect(detector_, SIGNAL(lowIndexValueChanged(int)), this, SLOT(onLowIndexValueChanged(int)));
	connect(detector_, SIGNAL(highIndexValueChanged(int)), this, SLOT(onHighIndexValueChanged(int)));
	connect(detector_, SIGNAL(eVPerBinChanged(double)), this, SLOT(onDetectorEvPerBinChanged(double)));
	if(detector_->isConnected())
		onDetectorConnected(detector_->isConnected());

	page1->setLayout(page1VL);
	page2->setLayout(page2VL);
	editModeTab->addTab(page1, QString("eV Mode"));
	editModeTab->addTab(page2, QString("bin Mode"));
	mainVL->addWidget(editModeTab);
	setLayout(mainVL);
}

CLSAmptekDetectorROIView::~CLSAmptekDetectorROIView(){}

void CLSAmptekDetectorROIView::onDetectorConnected(bool isConnected){
	if(!isConnected)
		return;

	for(int x = 0; x < 8; x++){
		onLowIndexValueChanged(x);
		onHighIndexValueChanged(x);
	}

	//Set title of the window to match that of the detector:
	this->setWindowTitle(QString("%1 - ROIs").arg(this->detector_->name()));
}

void CLSAmptekDetectorROIView::onLowIndexValueChanged(int index){

	roiLowIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekLowROI(index)));
	roiLowIndexDoubleSpinBoxes_.at(index)->setValue(detector_->amptekLowROIEv(index));
}

void CLSAmptekDetectorROIView::onHighIndexValueChanged(int index){


	roiHighIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekHighROI(index)));
	roiHighIndexDoubleSpinBoxes_.at(index)->setValue(detector_->amptekHighROIEv(index));
}

void CLSAmptekDetectorROIView::onLowIndexLineEditEditingFinished(int index){
	roiLineEditingFinishedHelper(index);
}

void CLSAmptekDetectorROIView::onHighIndexLineEditEditingFinished(int index){
	roiLineEditingFinishedHelper(index);
}

void CLSAmptekDetectorROIView::onLowIndexDoubleSpinBoxEditingFinished(int index)
{
	roiSpinBoxEditingFinishedHelper(index);
}

void CLSAmptekDetectorROIView::onHighIndexDoubleSpinBoxEditingFinished(int index)
{
	roiSpinBoxEditingFinishedHelper(index);
}

void CLSAmptekDetectorROIView::onDetectorEvPerBinChanged(double newValue){
	roiEditModeConversionRateLabel->setText(QString("%1 ev/bin").arg(newValue));
	for(int iROI = 0; iROI < roiLowIndexDoubleSpinBoxes_.count(); iROI++)
	{
		roiLowIndexDoubleSpinBoxes_.at(iROI)->setSingleStep(detector_->eVPerBin());
		roiLowIndexDoubleSpinBoxes_.at(iROI)->setMaximum(1023 * detector_->eVPerBin());
		roiLowIndexDoubleSpinBoxes_.at(iROI)->setValue(detector_->amptekLowROIEv(iROI));

		roiHighIndexDoubleSpinBoxes_.at(iROI)->setSingleStep(detector_->eVPerBin());
		roiHighIndexDoubleSpinBoxes_.at(iROI)->setMaximum(1023 * detector_->eVPerBin());
		roiHighIndexDoubleSpinBoxes_.at(iROI)->setValue(detector_->amptekHighROIEv(iROI));

	}
}

void CLSAmptekDetectorROIView::roiLineEditingFinishedHelper(int index){
	bool convertedLowSuccessfully = false;
	bool convertedHighSuccessfully = false;

		int convertedLowValue = roiLowIndexLineEdits_.at(index)->text().toInt(&convertedLowSuccessfully);
		int convertedHighValue = roiHighIndexLineEdits_.at(index)->text().toInt(&convertedHighSuccessfully);
		if(convertedLowSuccessfully && convertedHighSuccessfully && convertedLowValue < 1024 && convertedHighValue < 1024)
		{
			detector_->setAmptekROI(index, convertedLowValue, convertedHighValue);
		}
		else
		{
			roiLowIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekLowROI(index)));
			roiHighIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekHighROI(index)));

		}
		if(!convertedLowSuccessfully)
			QToolTip::showText(roiLowIndexLineEdits_.at(index)->mapToGlobal(QPoint(roiLowIndexLineEdits_.at(index)->width(),0)), QString("Could not convert input to integer"));
		if(!convertedHighSuccessfully)
			QToolTip::showText(roiHighIndexLineEdits_.at(index)->mapToGlobal(QPoint(roiHighIndexLineEdits_.at(index)->width(),0)), QString("Could not convert input to integer"));
		if(convertedLowValue > 1023)
			QToolTip::showText(roiLowIndexLineEdits_.at(index)->mapToGlobal(QPoint(roiLowIndexLineEdits_.at(index)->width(),0)), QString("Input must be within range 0-1023"));
		if(convertedHighValue > 1023)
			QToolTip::showText(roiHighIndexLineEdits_.at(index)->mapToGlobal(QPoint(roiHighIndexLineEdits_.at(index)->width(),0)), QString("Input must be within range 0-1023"));

}

void CLSAmptekDetectorROIView::roiSpinBoxEditingFinishedHelper(int index)
{
	double inputLowValue = roiLowIndexDoubleSpinBoxes_.at(index)->value();
	double inputHighValue = roiHighIndexDoubleSpinBoxes_.at(index)->value();

	detector_->setAmptekROIbyEv(index, roiLowIndexDoubleSpinBoxes_.at(index)->value(), roiHighIndexDoubleSpinBoxes_.at(index)->value());

	if(!qFuzzyCompare(inputLowValue, detector_->amptekLowROIEv(index)))
		roiLowIndexDoubleSpinBoxes_.at(index)->setValue(detector_->amptekLowROIEv(index));

	if(!qFuzzyCompare(inputHighValue, detector_->amptekHighROIEv(index)))
		roiHighIndexDoubleSpinBoxes_.at(index)->setValue(detector_->amptekHighROIEv(index));

}

CLSAmptekDetectorConfigurationView::CLSAmptekDetectorConfigurationView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;

	QVBoxLayout *mainVL = new QVBoxLayout();
	QHBoxLayout *tempHBox;

	totalGainControlEditor_ = new AMControlEditor(detector_->totalGainControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Total Gain"));
	tempHBox->addWidget(totalGainControlEditor_);
	mainVL->addLayout(tempHBox);

	mcaChannelsControlEditor_ = new AMControlEditor(detector_->mcaChannelsControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("MCA Channels"));
	tempHBox->addWidget(mcaChannelsControlEditor_);
	mainVL->addLayout(tempHBox);

	pileUpRejectionControlEditor_ = new AMControlEditor(detector_->pileUpRejectionControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Pile Up Rejection"));
	tempHBox->addWidget(pileUpRejectionControlEditor_);
	mainVL->addLayout(tempHBox);

	thermoelectricCoolerControlEditor_ = new AMControlEditor(detector_->thermoelectricCoolerControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Cooler"));
	tempHBox->addWidget(thermoelectricCoolerControlEditor_);
	mainVL->addLayout(tempHBox);

	fastThresholdControlEditor_ = new AMControlEditor(detector_->fastThresholdControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Fast Threshold"));
	tempHBox->addWidget(fastThresholdControlEditor_);
	mainVL->addLayout(tempHBox);

	slowThresholdControlEditor_ = new AMControlEditor(detector_->slowThresholdControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Slow Threshold"));
	tempHBox->addWidget(slowThresholdControlEditor_);
	mainVL->addLayout(tempHBox);

	peakingTimeControlEditor_ = new AMControlEditor(detector_->peakingTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Peaking Time"));
	tempHBox->addWidget(peakingTimeControlEditor_);
	mainVL->addLayout(tempHBox);

	fastChannelPeakingTimeControlEditor_ = new AMControlEditor(detector_->fastChannelPeakingTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(new QLabel("Fast Peaking Time"));
	tempHBox->addWidget(fastChannelPeakingTimeControlEditor_);
	mainVL->addLayout(tempHBox);

	setLayout(mainVL);
}

CLSAmptekDetectorConfigurationView::~CLSAmptekDetectorConfigurationView(){}

