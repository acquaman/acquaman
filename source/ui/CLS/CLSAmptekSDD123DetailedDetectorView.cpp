#include "CLSAmptekSDD123DetailedDetectorView.h"

#include <QLineEdit>
#include <QSignalMapper>
#include <QComboBox>

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
	//rightLayout_->addWidget(eVPerBinDoubleSpinBox_);
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

	tempLabel = new QLabel("Fast Counts");
	fastCountsControlEditor_ = new AMControlEditor(amptekDetector_->fastCountsControl(this));
	averageFastCountsControlEditor_ = new AMControlEditor(amptekDetector_->averageFastCountsControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(fastCountsControlEditor_);
	tempHBox->addWidget(averageFastCountsControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Slow Counts");
	slowCountsControlEditor_ = new AMControlEditor(amptekDetector_->slowCountsControl(this));
	averageSlowCountsControlEditor_ = new AMControlEditor(amptekDetector_->averageSlowCountsControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(slowCountsControlEditor_);
	tempHBox->addWidget(averageSlowCountsControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Accumulation");
	accumulationTimeControlEditor_ = new AMControlEditor(amptekDetector_->accumulationTimeControl(this));
	averageAccumulationTimeControlEditor_ = new AMControlEditor(amptekDetector_->averageAccumulationTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(accumulationTimeControlEditor_);
	tempHBox->addWidget(averageAccumulationTimeControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Real Time");
	realTimeControlEditor_ = new AMControlEditor(amptekDetector_->realTimeControl(this));
	averageRealTimeControlEditor_ = new AMControlEditor(amptekDetector_->averageRealTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(realTimeControlEditor_);
	tempHBox->addWidget(averageRealTimeControlEditor_);
	rightLayout_->addLayout(tempHBox);

	tempLabel = new QLabel("Live Time");
	liveTimeControlEditor_ = new AMControlEditor(amptekDetector_->liveTimeControl(this));
	averageLiveTimeControlEditor_ = new AMControlEditor(amptekDetector_->averageLiveTimeControl(this));
	tempHBox = new QHBoxLayout();
	tempHBox->addWidget(tempLabel);
	tempHBox->addWidget(liveTimeControlEditor_);
	tempHBox->addWidget(averageLiveTimeControlEditor_);
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
	isInEvMode_ = true;
	QVBoxLayout *mainVL = new QVBoxLayout();

	roiLowIndexLineEditsMapper_ = new QSignalMapper();
	roiHighIndexLineEditsMapper_ = new QSignalMapper();



	//Set up header widgets (includes edit mode (eV or bin) and current detector eV->bin conversion)
	QComboBox *roiEditModeComboBox;
	QLabel* roiEditModeLabel;
	QHBoxLayout* headerHBoxLayout;

	headerHBoxLayout = new QHBoxLayout();
	roiEditModeLabel = new QLabel(QString("Set ROIs by:"));
	roiEditModeConversionRateLabel = new QLabel();
	if(detector_->isConnected())
		roiEditModeConversionRateLabel->setText(QString("%1 eV/bin").arg(detector_->eVPerBin()));
	roiEditModeComboBox = new QComboBox();
	roiEditModeComboBox->addItem(QString("eV"));
	roiEditModeComboBox->addItem(QString("bin"));

	headerHBoxLayout->addWidget(roiEditModeLabel);
	headerHBoxLayout->addWidget(roiEditModeComboBox);
	headerHBoxLayout->addWidget(roiEditModeConversionRateLabel);
	mainVL->addLayout(headerHBoxLayout);

	//Set up widgets for each ROI
	QLabel *oneNameLabel;
	QLineEdit *oneLowIndexLineEdit;
	QLineEdit *oneHighIndexLineEdit;
	QHBoxLayout *oneHBoxLayout;

	for(int x = 0; x < 8; x++){
		oneNameLabel = new QLabel(QString("ROI %1").arg(x+1));
		oneLowIndexLineEdit = new QLineEdit();
		oneHighIndexLineEdit = new QLineEdit();

		roiLowIndexLineEdits_.append(oneLowIndexLineEdit);
		roiHighIndexLineEdits_.append(oneHighIndexLineEdit);

		roiLowIndexLineEditsMapper_->setMapping(oneLowIndexLineEdit, x);
		roiHighIndexLineEditsMapper_->setMapping(oneHighIndexLineEdit, x);

		connect(oneLowIndexLineEdit, SIGNAL(editingFinished()), roiLowIndexLineEditsMapper_, SLOT(map()));
		connect(oneHighIndexLineEdit, SIGNAL(editingFinished()), roiHighIndexLineEditsMapper_, SLOT(map()));

		oneHBoxLayout = new QHBoxLayout();
		oneHBoxLayout->addWidget(oneNameLabel);
		oneHBoxLayout->addWidget(oneLowIndexLineEdit);
		oneHBoxLayout->addWidget(oneHighIndexLineEdit);

		mainVL->addLayout(oneHBoxLayout);
	}
	connect(roiLowIndexLineEditsMapper_, SIGNAL(mapped(int)), this, SLOT(onLowIndexLineEditEditingFinished(int)));
	connect(roiHighIndexLineEditsMapper_, SIGNAL(mapped(int)), this, SLOT(onHighIndexLineEditEditingFinished(int)));
	connect(roiEditModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onRoiEditModeComboBoxIndexChanged(int)));

	connect(detector_, SIGNAL(connected(bool)), this, SLOT(onDetectorConnected(bool)));
	connect(detector_, SIGNAL(lowIndexValueChanged(int)), this, SLOT(onLowIndexValueChanged(int)));
	connect(detector_, SIGNAL(highIndexValueChanged(int)), this, SLOT(onHighIndexValueChanged(int)));
	connect(detector_, SIGNAL(eVPerBinChanged(double)), this, SLOT(onDetectorEvPerBinChanged(double)));
	if(detector_->isConnected())
		onDetectorConnected(detector_->isConnected());

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
	//TODO: Check mode (eV or bin) and set text value accordingly
	//if(this->isInEvMode_)
		//roiLowIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekLowROIEv(index)));
	//else
		roiLowIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekLowROI(index)));
}

void CLSAmptekDetectorROIView::onHighIndexValueChanged(int index){
	//TODO: Check mode (eV or bin) and set text value accordingly
	//if(this->isInEvMode)
		//roiHighIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekHighROIEv(index)));
	//else
	roiHighIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekHighROI(index)));
}

void CLSAmptekDetectorROIView::onLowIndexLineEditEditingFinished(int index){
	roiEditingFinishedHelper(index);
}

void CLSAmptekDetectorROIView::onHighIndexLineEditEditingFinished(int index){
	roiEditingFinishedHelper(index);
}

void CLSAmptekDetectorROIView::onDetectorEvPerBinChanged(double newValue){
	roiEditModeConversionRateLabel->setText(QString("%1 ev/bin").arg(newValue));
}

void CLSAmptekDetectorROIView::onRoiEditModeComboBoxIndexChanged(int newIndex){
	isInEvMode_ = (newIndex == 0);
	//TODO: Change displayed values in the LineEdits to use the right value units (eV or bin)
	for (int iLineEdit = 0; iLineEdit < this->roiLowIndexLineEdits_.count(); iLineEdit++)
	{
		if(isInEvMode_)
		{
			//roiHighIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekHighROIEv(index)));
			qDebug() << QString("ROI %1 Value: %2 eV to %3 eV").arg(iLineEdit+1).arg(detector_->amptekLowROIEv(iLineEdit)).arg(detector_->amptekHighROIEv(iLineEdit));
		}
		else
		{
			//roiHighIndexLineEdits_.at(index)->setText(QString("%1").arg(detector_->amptekHighROI(index)));
			qDebug() << QString("ROI %1 Value: %2 to %3").arg(iLineEdit+1).arg(detector_->amptekLowROI(iLineEdit)).arg(detector_->amptekHighROI(iLineEdit));
		}
	}
}

void CLSAmptekDetectorROIView::roiEditingFinishedHelper(int index){
	//TODO: Get current mode (eV or bin) and if eV convert to bin before settingROI
	bool convertedLowSuccessfully = false;
	bool convertedHighSuccessfully = false;
	//if(this->isInEvMode_)
	//{
		int convertedLowValue = roiLowIndexLineEdits_.at(index)->text().toInt(&convertedLowSuccessfully);
		int convertedHighValue = roiHighIndexLineEdits_.at(index)->text().toInt(&convertedHighSuccessfully);
		if(convertedLowSuccessfully && convertedHighSuccessfully)
			detector_->setAmptekROI(index, convertedLowValue, convertedHighValue);
	//}
	//else
	//{
		//double convertedLowValue = roiLowIndexLineEdits_.at(index)->text().toDouble(&convertedLowSuccessfully);
		//double convertedHighValue = roiHighIndexLineEdits_.at(index)->text().toDobuel(&convertedHighSuccessfully);
		//if(convertedLowValue && convertedHighSuccessfully)
			//detector_->setAmptekROIbyEv(index, convertedLowValue, convertedHighValue);
	//}


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
